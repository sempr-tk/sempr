#ifndef SEMPR_ENTITY_SPATIAL_MULTIPOINT_HPP_
#define SEMPR_ENTITY_SPATIAL_MULTIPOINT_HPP_

#include <sempr/entity/spatial/Collection.hpp>
#include <geos/geom/MultiPoint.h>

namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
    Container class for GEOM MultiPoint
*/
#pragma db object
class MultiPoint : public Collection {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<MultiPoint>;

    MultiPoint();
    MultiPoint(const core::IDGenBase*);
    virtual ~MultiPoint();

    const geom::MultiPoint* geometry() override;

    void setCoordinates(const std::vector<geom::Coordinate>& coordinates);

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    MultiPoint::Ptr clone() const;

private:
    friend class odb::access;

    virtual MultiPoint* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_MULTIPOINT_HPP_ */
