#ifndef SEMPR_ENTITY_SPATIAL_UNIVERSALTRANSVERSEMERCATORCS_HPP_
#define SEMPR_ENTITY_SPATIAL_UNIVERSALTRANSVERSEMERCATORCS_HPP_

#include <sempr/entity/spatial/reference/ProjectionCS.hpp>
#include <sempr/entity/spatial/filter/ProjectionFilter.hpp>

namespace sempr { namespace entity {

/**
 *  Coordinate system of a local tangent plane of a given origin (x/y/z) position.
 *  Use WGS84 as reference ellipsoid.
*/

#pragma db object
class UniversalTransverseMercatorCS : public ProjectionCS {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<UniversalTransverseMercatorCS>;

    UniversalTransverseMercatorCS(int zone, bool north = true);
    UniversalTransverseMercatorCS(int zone, bool north, const core::IDGenBase*);

    bool isEqual(const GlobalCS::Ptr other) override;

    int getZone() const override;
    bool isNorth() const override;

protected:
    UniversalTransverseMercatorCS();

    virtual FilterPtr forward() const;

    virtual FilterPtr reverse() const;

private:
    friend class odb::access;

    const int zone_;
    const bool north_;
};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_UNIVERSALTRANSVERSEMERCATORCS_HPP_ */
