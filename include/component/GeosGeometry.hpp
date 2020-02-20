#ifndef SEMPR_COMPONENT_GEOSGEOMETRY_HPP_
#define SEMPR_COMPONENT_GEOSGEOMETRY_HPP_

#include <geos/geom/Geometry.h>

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
    virtual const geos::geom::Geometry* geometry() const = 0;
    virtual geos::geom::Geometry* geometry() = 0;

};

/**
    The actual GeosGeometry containing data, in the form of a
    geos::geom::Geometry*.
*/
class GeosGeometry : public GeosGeometryInterface {
    geos::geom::Geometry* geometry_;
public:
    /**
        Constructs a GeosGeometry containing a given geos::geom::Geometry.
        Takes ownership of geometry.
    */
    GeosGeometry(geos::geom::Geometry* geometry);

    /**
        Creates an empty geometry.
    */
    GeosGeometry();

    ~GeosGeometry();

    geos::geom::Geometry* geometry() override;
    const geos::geom::Geometry* geometry() const override;

    /**
        Sets a new geometry for this component. Frees the old geometry.
    */
    void setGeometry(geos::geom::Geometry* geometry);
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

#endif /* include guard: SEMPR_COMPONENT_GEOSGEOMETRY_HPP_ */
