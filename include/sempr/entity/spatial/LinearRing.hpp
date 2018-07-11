#ifndef SEMPR_ENTITY_SPATIAL_LINEARRING_HPP_
#define SEMPR_ENTITY_SPATIAL_LINEARRING_HPP_

#include <sempr/entity/spatial/Geometry.hpp>
#include <sempr/entity/spatial/LineString.hpp>
#include <geos/geom/LinearRing.h>

#include <geos/geom/CoordinateArraySequenceFactory.h>

namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
    Container class for GEOM LineRing
*/
#pragma db object
class LinearRing : public LineString {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<LinearRing>;

    LinearRing();
    LinearRing(const core::IDGenBase*);
    virtual ~LinearRing();

    const geom::LinearRing* geometry() override;

    void setCoordinates(std::vector<geom::Coordinate>& coordinates);

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    LinearRing::Ptr clone() const;

private:
    friend class odb::access;

    virtual LinearRing* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LINEARRING_HPP_ */
