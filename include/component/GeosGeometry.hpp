#ifndef SEMPR_COMPONENT_GEOSGEOMETRY_HPP_
#define SEMPR_COMPONENT_GEOSGEOMETRY_HPP_

#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>

#include "../Component.hpp"

namespace sempr {

/**
    The GeosGeometryInterface is a very simple interface to geometry components
    that are based on the geos::geom::Geometry. The general idea is to mainly
    use GeosGeometry-components, which are wrapper for geos::geom::Geometry*,
    since it is often not known what exact type of geometry an operation will
    yield. E.g. the intersection of two polygons can be empty, a point, a line,
    a polygon, multiple polygons, ...
    But instead of implemeting only the GeosGeometry, this interface leaves us
    the opportunity to implement more specific components if we so desire.
*/
class GeosGeometryInterface : public Component {
public:
    using Ptr = std::shared_ptr<GeosGeometryInterface>;
    virtual const geos::geom::Geometry* geometry() const = 0;
    virtual geos::geom::Geometry* geometry() = 0;

};

/**
    The actual GeosGeometry containing data, in the form of a
    geos::geom::Geometry*.
*/
class GeosGeometry : public GeosGeometryInterface {
    SEMPR_COMPONENT

    std::unique_ptr<geos::geom::Geometry> geometry_;
public:
    /**
        Constructs a GeosGeometry containing a given geos::geom::Geometry.
        Takes ownership of geometry.
    */
    GeosGeometry(geos::geom::Geometry* geometry);
    GeosGeometry(std::unique_ptr<geos::geom::Geometry> geometry);

    /**
        Creates an empty geometry.
    */
    GeosGeometry();

    ~GeosGeometry();

    geos::geom::Geometry* geometry() override;
    const geos::geom::Geometry* geometry() const override;

    /**
        Sets a new geometry for this component. Delete the old geometry and takes the ownership.
    */
    void setGeometry(std::unique_ptr<geos::geom::Geometry> geometry);
    void setGeometry(geos::geom::Geometry* geometry);


    /**
        Serialization with cereal
    */
    template <class Archive>
    void save(Archive& ar) const
    {
        // configure a writer
        geos::io::WKTWriter writer;
        int dim = geometry_->getCoordinateDimension();
        writer.setOutputDimension(dim);

        // create the wkt string
        std::string wkt = writer.writeFormatted(geometry_.get());

        // save the string (and the base class)
        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("wkt", wkt) );
    }

    template <class Archive>
    void load(Archive& ar)
    {
        // load the wkt string (and the base class)
        std::string wkt;
        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("wkt", wkt) );

        // create a wkt reader
        const geos::geom::GeometryFactory& factory = *geos::geom::GeometryFactory::getDefaultInstance();
        geos::io::WKTReader reader(factory);

        // parse the string
        std::unique_ptr<geos::geom::Geometry> g(reader.read(wkt));

        // set the new geometry
        setGeometry(std::move(g));

    }
};


// specialization for sempr::ComponentName<class C>
template <>
struct ComponentName<GeosGeometryInterface> {
    static constexpr const char* value = "GeosGeomI";
};

template <>
struct ComponentName<GeosGeometry> {
    static constexpr const char* value = "GeosGeom";
};

}

CEREAL_REGISTER_TYPE(sempr::GeosGeometry)

// also, register a to_string function to display GeosGeometryInterface
// in the rete network
#include <rete-core/Util.hpp>
namespace rete { namespace util {

template <> std::string to_string(const sempr::GeosGeometryInterface& c);
template <> std::string to_string(const sempr::GeosGeometryInterface::Ptr& c);

}}

#endif /* include guard: SEMPR_COMPONENT_GEOSGEOMETRY_HPP_ */
