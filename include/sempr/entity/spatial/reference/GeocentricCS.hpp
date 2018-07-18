#ifndef SEMPR_ENTITY_SPATIAL_GEOCENTRICCS_HPP_
#define SEMPR_ENTITY_SPATIAL_GEOCENTRICCS_HPP_

#include <sempr/entity/spatial/reference/GlobalCS.hpp>
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

    /**
        Create a geographic coordinate system based on a well known text,
        e.g. "WGS84", "WGS72", "NAD27", "NAD83" or any "EPSG:n" that points to a geographic
        reference frame.
    */
    GeocentricCS();
    GeocentricCS(const core::IDGenBase*);

private:
    friend class odb::access;

};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEOCENTRICCS_HPP_ */
