#ifndef SEMPR_ENTITY_SPATIAL_GEODETICCS_HPP_
#define SEMPR_ENTITY_SPATIAL_GEODETICCS_HPP_

#include <sempr/entity/spatial/GlobalCS.hpp>
//#include <ogr_spatialref.h>

namespace sempr { namespace entity {

/**
    A geodetic reference system, e.g. WGS84 or GRS80.
    It is also a root of transformations.
*/
#pragma db object
class GeodeticCS : public GlobalCS {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<GeodeticCS>;

    /**
        Create a geographic coordinate system based on a well known text,
        e.g. "WGS84" or "GRS80" as points to a geographic reference frame.
    */
    GeodeticCS(GeodeticReference reference);
    GeodeticCS(GeodeticReference reference, const core::IDGenBase*);

private:
    friend class odb::access;

    GeodeticCS(); // just for ODB.

    void initParameter(GeodeticReference reference);
};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEODETICCS_HPP_ */
