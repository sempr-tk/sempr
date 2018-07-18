#include <sempr/entity/spatial/reference/GeodeticCS.hpp>
#include <GeodeticCS_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeodeticCS)

GeodeticCS::GeodeticCS()
    : GeodeticCS(new core::IDGen<GeodeticCS>())
{
}

GeodeticCS::GeodeticCS(const core::IDGenBase* idgen)
    : GlobalCS(idgen)
{
    this->setDiscriminator<GeodeticCS>();
}



}}
