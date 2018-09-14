#ifndef SEMPR_ENTITY_SPATIAL_LOCALTANGENTPLANECS_HPP_
#define SEMPR_ENTITY_SPATIAL_LOCALTANGENTPLANECS_HPP_

#include <sempr/entity/spatial/reference/GeocentricCS.hpp>
#include <sempr/entity/spatial/filter/GeocentricFilter.hpp>

namespace sempr { namespace entity {

/**
 *  Coordinate system of a local tangent plane of a given origin (x/y/z) position.
 *  Use WGS84 as reference ellipsoid.
*/

#pragma db object
class LocalTangentPlaneCS : public GeocentricCS {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<LocalTangentPlaneCS>;

    LocalTangentPlaneCS(const geom::Coordinate& origin);
    LocalTangentPlaneCS(double lat0, double lon0, double h0 = 0);
    LocalTangentPlaneCS(double lat0, double lon0, double h0, const core::IDGenBase*);

    bool isEqual(const SpatialReference::Ptr other) const override;

protected:
    LocalTangentPlaneCS();

    virtual FilterPtr forward() const override;

    virtual FilterPtr reverse() const override;

private:
    friend class odb::access;

    const double lat0_, lon0_, h0_;     //the origin of the coordinate system
};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LOCALTANGENTPLANECS_HPP_ */
