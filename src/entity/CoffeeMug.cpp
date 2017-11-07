#include <sempr/entity/CoffeeMug.hpp>
#include <CoffeeMug_odb.h>


namespace sempr { namespace entity {

CoffeeMug::CoffeeMug()
{
    setDiscriminator<CoffeeMug>();
    temperature_ = 100;
}

} /* entity */
} /* sempr */
