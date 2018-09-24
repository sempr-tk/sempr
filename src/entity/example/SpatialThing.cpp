#include <sempr/entity/example/SpatialThing.hpp>
#include <SpatialThing_odb.h>


namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(SpatialThing)

SpatialThing::SpatialThing(const core::IDGenBase* idgen)
    : Entity(idgen)
{
    setDiscriminator<SpatialThing>();
}

SpatialThing::SpatialThing() :
    SpatialThing(new core::IDGen<SpatialThing>())
{
}


} /* entity */
} /* sempr */
