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
    _type = TREES;
}

Trees::Trees(std::string name, const core::IDGenBase* idgen) :
    AgriEntity(name, idgen)
{
    setDiscriminator<Trees>();
    _type = TREES;
}

Trees::~Trees()
{

}

} } }
