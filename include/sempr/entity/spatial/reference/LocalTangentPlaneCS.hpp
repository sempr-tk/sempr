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
    LocalTangentPlaneCS(const geom::Coordinate& origin, const core::IDGenBase*);

    bool isEqual(const GlobalCS::Ptr other) override;

protected:

    virtual FilterPtr forward() const;

    virtual FilterPtr reverse() const;

private:
    friend class odb::access;

    const geom::Coordinate origin_;
};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LOCALTANGENTPLANECS_HPP_ */
