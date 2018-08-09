#ifndef SEMPR_ENTITY_SPATIAL_EARTHCENTEREDCS_HPP_
#define SEMPR_ENTITY_SPATIAL_EARTHCENTEREDCS_HPP_

#include <sempr/entity/spatial/reference/GeocentricCS.hpp>
#include <sempr/entity/spatial/filter/GeocentricFilter.hpp>
//#include <ogr_spatialref.h>

namespace sempr { namespace entity {


/**
    ECEF Coordinate System with the root (0, 0, 0) defined as the center of mass of the earth.
    Use WGS84 as reference ellipsoid.
*/
#pragma db object
class EarthCenteredCS : public GeocentricCS {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<EarthCenteredCS>;

    EarthCenteredCS();
    EarthCenteredCS(const core::IDGenBase*);

protected:

    virtual FilterPtr forward() const override;

    virtual FilterPtr reverse() const override;

private:
    friend class odb::access;

};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_EARTHCENTEREDCS_HPP_ */
