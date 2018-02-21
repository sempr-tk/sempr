#ifndef SEMPR_ENTITY_SPATIAL_POINT_HPP_
#define SEMPR_ENTITY_SPATIAL_POINT_HPP_

#include <sempr/entity/spatial/Geometry.hpp>
#include <ogr_geometry.h>


namespace sempr { namespace entity {

/**
    A base class for all geometry entities. Contains methods to manage spatial reference systems
    but no concrete geometry -- just a pure virtual OGRGeometry* geometry() method to be implemented
    by the derived classes (point, polygon, ...).
*/
#pragma db object
class Point : public Geometry {
public:
    using Ptr = std::shared_ptr<Point>;
    ENTITY_DEFAULT_EVENT_METHODS(Point, Geometry);

    Point();
    Point(const core::IDGenBase*);
    virtual ~Point();

    OGRPoint* geometry() override;

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    Point::Ptr clone() const;

private:
    friend class odb::access;
    #pragma db type("TEXT")
    OGRPoint* geometry_;

    virtual Point* raw_clone() const override;
};

}}


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_POINT_HPP_ */
