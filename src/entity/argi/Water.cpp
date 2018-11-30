#include <sempr/entity/agri/Water.hpp>
#include "Water_odb.h"

namespace sempr { namespace entity { namespace agri {

SEMPR_ENTITY_SOURCE(Water)

Water::Water() : Water(new core::IDGen<Water>())
{
}

Water::Water(const core::IDGenBase* idgen) : AgriEntity(idgen)
{
    setDiscriminator<Water>();
    _type = WATER;
}

Water::Water(std::string name, const core::IDGenBase* idgen) :
    AgriEntity(name, idgen)
{
    setDiscriminator<Water>();
    _type = WATER;
}

Water::~Water()
{

}

} } }
