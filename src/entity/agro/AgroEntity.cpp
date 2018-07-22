#include <sempr/entity/agro/AgroEntity.hpp>
#include <AgroEntity_odb.h>


namespace sempr { namespace entity{ namespace agro {

SEMPR_ENTITY_SOURCE(AgroEntity)

AgroEntity::AgroEntity() : AgroEntity(new core::IDGen<AgroEntity>())
{
}

AgroEntity::AgroEntity(const core::IDGenBase* idgen) : Entity(idgen)
{
    geometry_ = static_cast<OGRPolygon*>(OGRGeometryFactory::createGeometry(wkbPolygon));

    m_bb.minX = 0.0;
    m_bb.maxX = 0.0;
    m_bb.minY = 0.0;
    m_bb.maxY = 0.0;
    m_bb.minZ = 0.0;
    m_bb.maxZ = 0.0;
}

AgroEntity::~AgroEntity()
{
    OGRGeometryFactory::destroyGeometry(geometry_);
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

inline void AgroEntity::checkBounds(const OGRPoint *point) {
    if(point->getX() < m_bb.minX) m_bb.minX = point->getX();
    if(point->getX() > m_bb.maxX) m_bb.maxX = point->getX();
    if(point->getY() < m_bb.minY) m_bb.minY = point->getZ();
    if(point->getY() > m_bb.maxY) m_bb.maxY = point->getY();
    if(point->getZ() < m_bb.minZ) m_bb.minZ = point->getZ();
    if(point->getZ() > m_bb.maxZ) m_bb.maxZ = point->getZ();
}

void AgroEntity::calculateBoundingBox()
{
    OGRPointIterator* it = geometry_->getExteriorRing()->getPointIterator();
    OGRPoint* p;

    while(it->getNextPoint(p))
    {
        checkBounds(p);

    }
    OGRPointIterator::destroy(it);
    //OGRGeometryFactory::destroyGeometry(p);
}



}}}
