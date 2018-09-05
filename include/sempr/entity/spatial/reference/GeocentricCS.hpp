#ifndef SEMPR_ENTITY_SPATIAL_GEOCENTRICCS_HPP_
#define SEMPR_ENTITY_SPATIAL_GEOCENTRICCS_HPP_

#include <sempr/entity/spatial/reference/GlobalCS.hpp>
#include <sempr/entity/spatial/filter/GeocentricFilter.hpp>
//#include <ogr_spatialref.h>

namespace sempr { namespace entity {


/**
    A cartesian geocentric reference system, e.g. ECEF.
    It is also a root of transformations.
*/
#pragma db object
class GeocentricCS : public GlobalCS {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<GeocentricCS>;

    virtual FilterList to(const GlobalCS::Ptr other) const override;

protected:

    virtual FilterPtr forward() const override;

    virtual FilterPtr reverse() const override;


    GeocentricCS();
    GeocentricCS(const core::IDGenBase*);

private:
    friend class odb::access;

};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEOCENTRICCS_HPP_ */
