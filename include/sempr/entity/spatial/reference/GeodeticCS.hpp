#ifndef SEMPR_ENTITY_SPATIAL_GEODETICCS_HPP_
#define SEMPR_ENTITY_SPATIAL_GEODETICCS_HPP_

#include <sempr/entity/spatial/reference/GlobalCS.hpp>
//#include <ogr_spatialref.h>

namespace sempr { namespace entity {

/**
    A geodetic reference system in WGS84.
    It is also a root of transformations.
*/
#pragma db object
class GeodeticCS : public GlobalCS {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<GeodeticCS>;

    GeodeticCS();
    GeodeticCS(const core::IDGenBase*);

private:
    friend class odb::access;
};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEODETICCS_HPP_ */
