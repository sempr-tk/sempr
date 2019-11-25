#ifndef SEMPR_TRAITS_SQLITE_GEOM_GEOMETRY_HXX
#define SEMPR_TRAITS_SQLITE_GEOM_GEOMETRY_HXX

#include <odb/sqlite/traits.hxx>

#include <sempr/entity/spatial/Geometry.hpp>

#include <geos/geom/GeometryFactory.h>

#include <cstddef> // std::size_t
#include <cstring> // std::strncmp, std::memcpy
#include <sstream>
#include <iostream>

#include <type_traits>

namespace odb { namespace sqlite {

namespace geom = geos::geom;

/**
    These are the traits for storing GEOM geometry in the database. The geometry will be
    saved as WKB and loaded through the GeometryFactory.

    This version is to save geometries as binary blobs (wkb)
*/
template <>
class value_traits<geom::Geometry*, id_blob>
{
public:
    typedef geom::Geometry* value_type;
    typedef geom::Geometry* query_type;
    typedef details::buffer image_type;

    /**
        geometry from image,
        templated for different geometry-subclasses:
            enable_if --> the function only exists for G that fullfill the criterion:
                geom::Geometry is_base_of G
    */
    template <class G, typename = typename std::enable_if<std::is_base_of<geom::Geometry, G>::value, G>::type>
    static void
    set_value(G*& geometry, const image_type& b, std::size_t n, bool is_null)
    {
        if (!is_null) {
            // Note: I was tempted to do something like
            // geometry = new G();
            // geometry->ImportFromWkb(...);
            // But this is wrong!
            // a Polygon* may also point to a Triangle, a Curve* also to a CompoundCurve, SimpleCurve,
            // LineString, LinearRing, ...
            // So, we really need to use the Factory to create the correct object type
            // for us. We can still cast the resulting Geometry-ptr to the specific type as given
            // though the template parameter: We know that the geometry is of this or a derived type
            // since thats the type of pointer that is persisted for the class it's stored in.

            // parse the geometry from wkb
            //std::basic_string<char> buffer((char*)(b.data()));
            std::basic_string<char> buffer(b.data(), n);
            auto tmpGeom = sempr::entity::Geometry::importFromWKB(buffer);

            // (Let's assume that the entities create an empty geometry in their ctor and assign
            // it to the pointer we are currently trying to "load".)
            // The factory might return null if the geometry is "EMPTY" (e.g. "POINT EMPTY").
            // In that case: just do nothing and keep the previously created, empty geometry
            // (Which should be created through the GeometryFactory inside the entity to avoid
            // cross-heap-problems!)
            // else: free the old geometry, assign the new.

            if (tmpGeom) {
                auto factory = geom::GeometryFactory::getDefaultInstance();
                factory->destroyGeometry(geometry);
                geometry = dynamic_cast<G*>(tmpGeom);
            }

        } else {
            // assume that the geometry-ptr has already been set, either to NULL (in which case
            // a delete does nothing) or to previously in OGRGeometryFactory created geometry.
            // Since an explicit "NULL" is stored in the database, load a null, too.

            auto factory = geom::GeometryFactory::getDefaultInstance();
            factory->destroyGeometry(geometry);

            geometry = NULL;
        }
    }

    // image from geometry (--> save into database)
    template <class G, typename = typename std::enable_if<std::is_base_of<geom::Geometry, G>::value, G>::type>
    static void
    set_image(image_type& b, std::size_t& n, bool& is_null, G* const& geometry)
    {
        if (geometry == NULL) {
            is_null = true;
            n = 0;
            return;
        }

        is_null = false;

        auto wkb = sempr::entity::Geometry::exportToWKB(geometry);
        n = wkb.size();

        if (b.capacity() < n) {
            b.capacity(n);
        }

        //copy data to image
        std::memcpy(b.data(), wkb.data(), n);
    }
};


/**
    This version is to save geometries as text (wkt)
*/
template <>
class value_traits<geom::Geometry*, id_text>
{
public:
    typedef geom::Geometry* value_type;
    typedef geom::Geometry* query_type;
    typedef details::buffer image_type;

    // geometry from image
    template <class G, typename = typename std::enable_if<std::is_base_of<geom::Geometry, G>::value, G>::type>
    static void
    set_value(G*& geometry, const image_type& b, std::size_t n, bool is_null)
    {
        if (!is_null) {
            // (Let's assume that the entities create an empty geometry in their ctor and assign
            // it to the pointer we are currently trying to "load".)
            // The factory might return null if the geometry is "EMPTY" (e.g. "POINT EMPTY").
            // In that case: just do nothing and keep the previously created, empty geometry
            // (Which should be created through the GeometryFactory inside the entity to avoid
            // cross-heap-problems!)
            // else: free the old geometry, assign the new.

            char* buffer = const_cast<char*>(b.data());

            auto tmpGeom = sempr::entity::Geometry::importFromWKT(std::string(buffer));

            if (tmpGeom) {
                auto factory = geom::GeometryFactory::getDefaultInstance();
                factory->destroyGeometry(geometry);
                geometry = dynamic_cast<G*>(tmpGeom);

            }

        } else {
            // there was really a null-ptr stored. well then...

            auto factory = geom::GeometryFactory::getDefaultInstance();
            factory->destroyGeometry(geometry);

            geometry = NULL;
        }

    }

    // image from geometry (--> save into database)
    template <class G, typename = typename std::enable_if<std::is_base_of<geom::Geometry, G>::value, G>::type>
    static void set_image(image_type& b, std::size_t& n, bool& is_null, G* const& geometry)
    {
        if (geometry == NULL) {
            is_null = true;
            n = 0;
            return;
        }

        is_null = false;

        auto wkt = sempr::entity::Geometry::exportToWKT(geometry);

        n = wkt.size();

        if (b.capacity() < n) {
            b.capacity(n);
        }

        std::memcpy(b.data(), wkt.data(), n);

    }
};

// geometry-to-text
template <> class value_traits<geom::Point*, id_text> : public value_traits<geom::Geometry*, id_text> {};
template <> class value_traits<geom::GeometryCollection*, id_text> : public value_traits<geom::Geometry*, id_text> {};
template <> class value_traits<geom::Polygon*, id_text> : public value_traits<geom::Geometry*, id_text> {};
template <> class value_traits<geom::LineString*, id_text> : public value_traits<geom::Geometry*, id_text> {};
template <> class value_traits<geom::LinearRing*, id_text> : public value_traits<geom::Geometry*, id_text> {};
template <> class value_traits<geom::MultiPoint*, id_text> : public value_traits<geom::Geometry*, id_text> {};

// geometry-to-binary-blob
template <> class value_traits<geom::Point*, id_blob> : public value_traits<geom::Geometry*, id_blob> {};
template <> class value_traits<geom::GeometryCollection*, id_blob> : public value_traits<geom::Geometry*, id_blob> {};
template <> class value_traits<geom::Polygon*, id_blob> : public value_traits<geom::Geometry*, id_blob> {};
template <> class value_traits<geom::LineString*, id_blob> : public value_traits<geom::Geometry*, id_blob> {};

template <> class value_traits<geom::LinearRing*, id_blob> : public value_traits<geom::Geometry*, id_blob> {};

template <> class value_traits<geom::MultiPoint*, id_blob> : public value_traits<geom::Geometry*, id_blob> {};


} /* sqlite */

} /* odb */


#endif /* end of include guard: SEMPR_TRAITS_SQLITE_GEOM_GEOMETRY_HXX */
