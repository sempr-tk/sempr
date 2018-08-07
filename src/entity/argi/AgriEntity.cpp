#include <AgriEntity_odb.h>

namespace sempr { namespace entity{ namespace agri {

SEMPR_ENTITY_SOURCE(AgriEntity)

AgriEntity::AgriEntity() : AgriEntity(new core::IDGen<AgriEntity>())
{
}

AgriEntity::AgriEntity(const core::IDGenBase* idgen) : Entity(idgen)
{
    setDiscriminator<AgriEntity>();

    m_polygon = Polygon::Ptr(new Polygon());
    std::cout << id() << " created an Polygon, namely " << m_polygon->id() << '\n';

    registerChildEntity(m_polygon);
}

AgriEntity::~AgriEntity()
{
}


}}}
