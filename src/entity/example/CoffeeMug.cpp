#include <sempr/entity/example/CoffeeMug.hpp>
#include <CoffeeMug_odb.h>


namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(CoffeeMug)

CoffeeMug::CoffeeMug(const core::IDGenBase* idgen)
    : Entity(idgen)
{
    setDiscriminator<CoffeeMug>();
    temperature_ = 100;
}

CoffeeMug::CoffeeMug() : CoffeeMug(new core::IDGen<CoffeeMug>())
{
}


} /* entity */
} /* sempr */
