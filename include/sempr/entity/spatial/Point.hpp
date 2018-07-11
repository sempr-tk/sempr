#ifndef SEMPR_ENTITY_SPATIAL_POINT_HPP_
#define SEMPR_ENTITY_SPATIAL_POINT_HPP_

#include <sempr/entity/spatial/Geometry.hpp>
#include <geos/geom/Point.h>

namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
    A base class for all geometry entities. Contains methods to manage spatial reference systems
    but no concrete geometry -- just a pure virtual OGRGeometry* geometry() method to be implemented
    by the derived classes (point, polygon, ...).
*/
#pragma db object
class Point : public Geometry {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Point>;

    Point();
    Point(const core::IDGenBase*);
    virtual ~Point();

    geom::Point* geometry() override;

    void setCoordinate(const geom::Coordinate& coordinate);

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    Point::Ptr clone() const;

private:
    friend class odb::access;

    virtual Point* raw_clone() const override;
};

}}


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_POINT_HPP_ */
