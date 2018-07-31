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

    const geom::MultiPoint* getGeometry() const override;

    void setGeometry(geom::MultiPoint* geometry);

    void setCoordinates(const std::vector<geom::Coordinate>& coordinates);

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    MultiPoint::Ptr clone() const;

protected:
    geom::MultiPoint* geometry() const override;

private:
    friend class odb::access;

    #pragma db type("TEXT")
    geom::MultiPoint* geometry_;

    virtual MultiPoint* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_MULTIPOINT_HPP_ */
