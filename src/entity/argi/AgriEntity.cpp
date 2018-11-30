#include <AgriEntity_odb.h>

namespace sempr { namespace entity{ namespace agri {

SEMPR_ENTITY_SOURCE(AgriEntity)

AgriEntity::AgriEntity() : AgriEntity(new core::IDGen<AgriEntity>())
{
}

AgriEntity::AgriEntity(const core::IDGenBase* idgen) : AgriEntity("none", idgen)
{
}

AgriEntity::AgriEntity(std::string name, const core::IDGenBase* idgen) : _name(name), Entity(idgen)
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
