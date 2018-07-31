#include <sempr/entity/agro/AgroEntity.hpp>
#include <AgroEntity_odb.h>


namespace sempr { namespace entity{ namespace agro {

SEMPR_ENTITY_SOURCE(AgroEntity)

AgroEntity::AgroEntity() : AgroEntity(new core::IDGen<AgroEntity>())
{
}

AgroEntity::AgroEntity(const core::IDGenBase* idgen) : Entity(idgen)
{
    setDiscriminator<AgroEntity>();

    m_polygon = Polygon::Ptr(new Polygon());
    std::cout << id() << " created an Polygon, namely " << m_polygon->id() << '\n';

    registerChildEntity(m_polygon);
}

AgroEntity::~AgroEntity()
{
}


}}}
