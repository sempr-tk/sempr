#include <sempr/entity/spatial/MultiPoint.hpp>
#include <geos/geom/CoordinateArraySequenceFactory.h>
#include <MultiPoint_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(MultiPoint)

MultiPoint::MultiPoint() : MultiPoint(new core::IDGen<MultiPoint>())
{
}

MultiPoint::MultiPoint(const core::IDGenBase* idgen)
    : Collection(idgen)
{
    this->setDiscriminator<MultiPoint>();
    geometry_ = factory_->createMultiPoint();
}

MultiPoint::~MultiPoint()
{
    if (geometry_)
    {
        factory_->destroyGeometry(geometry_);
        geometry_ = nullptr;
    }
}

const geom::MultiPoint* MultiPoint::getGeometry() const
{
    return this->geometry();
}

geom::MultiPoint* MultiPoint::geometry() const
{
    return geometry_;
}

void MultiPoint::setGeometry(geom::MultiPoint* geometry)
{
    if (geometry_)
        factory_->destroyGeometry(geometry_);

    geometry_ = geometry;
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
    newInstance->setGeometry( dynamic_cast<geom::MultiPoint*>(geometry_->clone()) );
    
    return newInstance;
}

}}
