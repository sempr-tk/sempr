#ifndef SEMPR_TRAITS_SQLITE_GEOMETRY_HXX
#define SEMPR_TRAITS_SQLITE_GEOMETRY_HXX

#include <odb/sqlite/traits.hxx>
#include <ogr_geometry.h>
#include <cpl_conv.h>

#include <cstddef> // std::size_t
#include <cstring> // std::strncmp, std::memcpy
#include <sstream>
#include <iostream>

#include <type_traits>

namespace odb { namespace sqlite {

/**
    These are the traits for storing OGRGeometry-*pointer* in the database. The geometry will be
    saved as WKB and loaded through the OGRGeometryFactory. (This requires to be freed by using
    OGRGeometryFactory::destroyGeometry()!)

    This version is to save geometries as binary blobs (wkb)
*/
template <>
class value_traits<OGRGeometry*, id_blob>
{
public:
    typedef OGRGeometry* value_type;
    typedef OGRGeometry* query_type;
    typedef details::buffer image_type;

    /**
        geometry from image,
        templated for different geometry-subclasses:
            enable_if --> the function only exists for G that fullfill the criterion:
                OGRGeometry is_base_of G
    */
    template <class G, typename = typename std::enable_if<std::is_base_of<OGRGeometry, G>::value, G>::type>
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
            // So, we really need to use the OGRGeometryFactory to create the correct object type
            // for us. We can still cast the resulting Geometry-ptr to the specific type as given
            // though the template parameter: We know that the geometry is of this or a derived type
            // since thats the type of pointer that is persisted for the class it's stored in.

            // parse the geometry from wkb
            OGRGeometry* tmpGeo;
            OGRGeometryFactory::createFromWkb((unsigned char*)(b.data()), NULL, &tmpGeo, n, wkbVariantIso);

            // (Let's assume that the entities create an empty geometry in their ctor and assign
            // it to the pointer we are currently trying to "load".)
            // The factory might return null if the geometry is "EMPTY" (e.g. "POINT EMPTY").
            // In that case: just do nothing and keep the previously created, empty geometry
            // (Which should be created through the GeometryFactory inside the entity to avoid
            // cross-heap-problems!)
            // else: free the old geometry, assign the new.
            if (tmpGeo) {
                OGRGeometryFactory::destroyGeometry(geometry);
                geometry = static_cast<G*>(tmpGeo);
            }

        } else {
            // assume that the geometry-ptr has already been set, either to NULL (in which case
            // a delete does nothing) or to previously in OGRGeometryFactory created geometry.
            // Since an explicit "NULL" is stored in the database, load a null, too.
            OGRGeometryFactory::destroyGeometry(geometry);
            geometry = NULL;
        }
    }

    // image from geometry (--> save into database)
    template <class G, typename = typename std::enable_if<std::is_base_of<OGRGeometry, G>::value, G>::type>
    static void
    set_image(image_type& b, std::size_t& n, bool& is_null, const G*& geometry)
    {
        if (geometry == NULL) {
            is_null = true;
            n = 0;
            return;
        }

        is_null = false;
        n = geometry->WkbSize();

        if (b.capacity() < n) {
            b.capacity(n);
        }

        geometry->exportToWkb(wkbXDR, (unsigned char*)(b.data()), wkbVariantIso);
    }
};


/**
    This version is to save geometries as text (wkt)
*/
template <>
class value_traits<OGRGeometry*, id_text>
{
public:
    typedef OGRGeometry* value_type;
    typedef OGRGeometry* query_type;
    typedef details::buffer image_type;

    // geometry from image
    template <class G, typename = typename std::enable_if<std::is_base_of<OGRGeometry, G>::value, G>::type>
    static void
    set_value(G*& geometry, const image_type& b, std::size_t n, bool is_null)
    {
        if (!is_null) {
            // std::string tmp(b.data(), n);
            char* tmp = const_cast<char*>(b.data());
            OGRGeometry* tmpGeo;
            OGRGeometryFactory::createFromWkt(&tmp, NULL, &tmpGeo);

            // (Let's assume that the entities create an empty geometry in their ctor and assign
            // it to the pointer we are currently trying to "load".)
            // The factory might return null if the geometry is "EMPTY" (e.g. "POINT EMPTY").
            // In that case: just do nothing and keep the previously created, empty geometry
            // (Which should be created through the GeometryFactory inside the entity to avoid
            // cross-heap-problems!)
            // else: free the old geometry, assign the new.
            if (tmpGeo) {
                OGRGeometryFactory::destroyGeometry(geometry);
                geometry = static_cast<G*>(tmpGeo);
            }
        } else {
            // there was really a null-ptr stored. well then...
            OGRGeometryFactory::destroyGeometry(geometry);
            geometry = NULL;
        }

    }

    // image from geometry (--> save into database)
    template <class G, typename = typename std::enable_if<std::is_base_of<OGRGeometry, G>::value, G>::type>
    static void
    set_image(image_type& b, std::size_t& n, bool& is_null, G* const& geometry)
    {
        if (geometry == NULL) {
            is_null = true;
            n = 0;
            return;
        }

        is_null = false;
        // n = geometry->WkbSize();
        char* wkt;
        geometry->exportToWkt(&wkt, wkbVariantIso);

        {
            std::string tmp(wkt);
            n = tmp.size();
            if (b.capacity() < n) {
                b.capacity(n);
            }
            std::memcpy(b.data(), tmp.data(), n);
        }
        CPLFree(wkt);
    }
};

// geometry-to-text
template <> class value_traits<OGRPoint*, id_text> : public value_traits<OGRGeometry*, id_text> {};
template <> class value_traits<OGRGeometryCollection*, id_text> : public value_traits<OGRGeometry*, id_text> {};
template <> class value_traits<OGRCurvePolygon*, id_text> : public value_traits<OGRGeometry*, id_text> {};
template <> class value_traits<OGRPolygon*, id_text> : public value_traits<OGRGeometry*, id_text> {};
template <> class value_traits<OGRLineString*, id_text> : public value_traits<OGRGeometry*, id_text> {};

// geometry-to-binary-blob
template <> class value_traits<OGRPoint*, id_blob> : public value_traits<OGRGeometry*, id_blob> {};
template <> class value_traits<OGRGeometryCollection*, id_blob> : public value_traits<OGRGeometry*, id_blob> {};
template <> class value_traits<OGRCurvePolygon*, id_blob> : public value_traits<OGRGeometry*, id_blob> {};
template <> class value_traits<OGRPolygon*, id_blob> : public value_traits<OGRGeometry*, id_blob> {};
template <> class value_traits<OGRLineString*, id_blob> : public value_traits<OGRGeometry*, id_blob> {};


} /* sqlite */

} /* odb */


#endif /* end of include guard: SEMPR_TRAITS_SQLITE_GEOMETRY_HXX */
