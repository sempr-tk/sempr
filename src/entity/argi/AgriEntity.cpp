#include <AgriEntity_odb.h>
#include <sempr/entity/agri/AgriEntity.hpp>

namespace sempr { namespace entity{ namespace agri {

SEMPR_ENTITY_SOURCE(AgriEntity)

AgriEntity::AgriEntity() : AgriEntity(new core::IDGen<AgriEntity>())
{
}

AgriEntity::AgriEntity(const core::IDGenBase* idgen) : AgriEntity("none", idgen)
{
}

AgriEntity::AgriEntity(const std::string& name, const core::IDGenBase* idgen) : Entity(idgen), _name(name)
{
    setDiscriminator<AgriEntity>();

    _geometry = Polygon::Ptr(new Polygon());
    _map = RDFPropertyMap::Ptr(new RDFPropertyMap("SemanticData", this->discriminator() + "/"));

    registerChildEntity(_geometry);
    registerChildEntity(_map);
}

AgriEntity::~AgriEntity()
{
}


}}}
