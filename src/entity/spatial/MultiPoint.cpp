#include <sempr/entity/spatial/MultiPoint.hpp>
#include <geos/geom/CoordinateArraySequenceFactory.h>
#include <MultiPoint_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(MultiPoint)

MultiPoint::MultiPoint() : MultiPoint(new core::IDGen<MultiPoint>())
{
}

MultiPoint::MultiPoint(const core::IDGenBase* idgen)
    : GeometryCollection(idgen)
{
    this->setDiscriminator<MultiPoint>();
    geometry_ = factory_->createMultiPoint();
}

MultiPoint::~MultiPoint()
{
    factory_->destroyGeometry(geometry_);
    geometry_ = nullptr;
}

const geom::MultiPoint* MultiPoint::geometry() 
{
    return dynamic_cast<geom::MultiPoint*>(geometry_);
}

void MultiPoint::setCoordinates(const std::vector<geom::Coordinate>& coordinates)
{
    setGeometry(factory_->createMultiPoint(coordinates));
}

MultiPoint::Ptr MultiPoint::clone() const 
{
    // raw clone is virtual! :)
    return MultiPoint::Ptr(raw_clone());
}

MultiPoint* MultiPoint::raw_clone() const
{
    MultiPoint* newInstance = new MultiPoint();
    // set the same reference frame
    newInstance->setCS(this->getCS());

    // copy the geometry
    newInstance->geometry_ = geometry_->clone(); 
    
    return newInstance;
}

}}
