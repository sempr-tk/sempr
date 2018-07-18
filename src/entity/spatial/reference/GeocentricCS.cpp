#include <sempr/entity/spatial/reference/GeocentricCS.hpp>
#include <GeocentricCS_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeocentricCS)

GeocentricCS::GeocentricCS() : 
    GeocentricCS(new core::IDGen<GeocentricCS>())
{
}

GeocentricCS::GeocentricCS(const core::IDGenBase* idgen) : 
    GlobalCS(idgen)
{
    this->setDiscriminator<GeocentricCS>();
}

}}
