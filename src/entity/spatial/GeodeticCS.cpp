#include <sempr/entity/spatial/GeodeticCS.hpp>
#include <GeodeticCS_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeodeticCS)

GeodeticCS::GeodeticCS()
{
    // do nothing. this is just for odb to load from the database...
}

GeodeticCS::GeodeticCS(GeodeticReference reference)
    : GeodeticCS(reference, new core::IDGen<GeodeticCS>())
{
}

GeodeticCS::GeodeticCS(GeodeticReference reference, const core::IDGenBase* idgen)
    : GlobalCS(idgen)
{
    this->setDiscriminator<GeodeticCS>();
}



}}
