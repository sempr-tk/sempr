#include <sempr/entity/CoffeeMug.hpp>
#include <CoffeeMug_odb.h>


namespace sempr { namespace entity {

CoffeeMug::CoffeeMug(const core::IDGenBase& idgen)
    : Entity(idgen)
{
    setDiscriminator<CoffeeMug>();
    temperature_ = 100;
}

CoffeeMug::CoffeeMug() : CoffeeMug(core::IDGen<CoffeeMug>())
{
}


} /* entity */
} /* sempr */
