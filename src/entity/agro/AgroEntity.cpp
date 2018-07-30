//#include <sempr/entity/agro/AgroEntity.hpp>
#include <AgroEntity_odb.h>


namespace sempr { namespace entity{ namespace agro {

SEMPR_ENTITY_SOURCE(AgroEntity)

AgroEntity::AgroEntity() : AgroEntity(new core::IDGen<AgroEntity>())
{
}

AgroEntity::AgroEntity(const core::IDGenBase* idgen) : Polygon(idgen)
{
    this->setDiscriminator<AgroEntity>();
}

AgroEntity::~AgroEntity()
{
}

/*
bool AgroEntity::calculateIntersection(const Polygon::Ptr &polygon)
{
    if(m_polygon->Intersects(polygon))
    {
            return true;
    }
    return false;
}
*/


}}}
