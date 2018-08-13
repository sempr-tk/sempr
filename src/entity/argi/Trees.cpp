#include <sempr/entity/agri/Trees.hpp>
#include "Trees_odb.h"

namespace sempr { namespace entity { namespace agri {

SEMPR_ENTITY_SOURCE(Trees)

Trees::Trees() : Trees(new core::IDGen<Trees>())
{
}

Trees::Trees(const core::IDGenBase* idgen) : AgriEntity(idgen)
{
    setDiscriminator<Trees>();
}

Trees::Trees(std::string type, std::string identifier, const core::IDGenBase* idgen) :
    AgriEntity(idgen), m_type(type), m_identifier(identifier)
{
    setDiscriminator<Trees>();
}

Trees::~Trees()
{

}

} } }
