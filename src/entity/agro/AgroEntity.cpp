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

PointCloud::BoundingBox AgroEntity::boundingBox()
{
    PointCloud::BoundingBox bb;

    bb.minX = DBL_MAX;
    bb.maxX = DBL_MIN;
    bb.minY = DBL_MAX;
    bb.maxY = DBL_MIN;
    bb.minZ = DBL_MAX;
    bb.maxZ = DBL_MIN;

    std::cout << "what the heck" << geometry_->getExteriorRing()->getNumPoints() << std::endl;

    OGRPointIterator* it = geometry_->getExteriorRing()->getPointIterator();
    OGRPoint *p = new OGRPoint(); // fuck
    std::cout << "is therè a lack?" << std::endl;

    while(it->getNextPoint(p) == true)
    {
        std::cout << bb.minX;
        if(p->getX() < bb.minX) bb.minX = p->getX();
        if(p->getX() > bb.maxX) bb.maxX = p->getX();
        if(p->getY() < bb.minY) bb.minY = p->getY();
        if(p->getY() > bb.maxY) bb.maxY = p->getY();
        if(p->getZ() < bb.minZ) bb.minZ = p->getZ();
        if(p->getZ() > bb.maxZ) bb.maxZ = p->getZ();
        std::cout << "  >" << bb.minX << std::endl;
    }

    std::cout << "deleting" << std::endl;
    OGRPointIterator::destroy(it);
    std::cout << "deleted" << std::endl;

    OGRGeometryFactory::destroyGeometry(p);

    return bb;
}

}}}
