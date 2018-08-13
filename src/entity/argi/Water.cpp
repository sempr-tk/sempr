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
}

Water::Water(std::string type, std::string identifier, const core::IDGenBase* idgen) :
    AgriEntity(idgen), m_type(type), m_identifier(identifier)
{
    setDiscriminator<Water>();
}

Water::~Water()
{

}

} } }
