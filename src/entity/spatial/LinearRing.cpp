#include <sempr/entity/spatial/LinearRing.hpp>
#include <LinearRing_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(LinearRing)

LinearRing::LinearRing() : LinearRing(new core::IDGen<LinearRing>())
{
}

LinearRing::LinearRing(const core::IDGenBase* idgen)
    : Lineal(idgen)
{
    this->setDiscriminator<LinearRing>();
    geometry_ = factory_->createLinearRing();
}

LinearRing::~LinearRing()
{
    factory_->destroyGeometry(geometry_);
    geometry_ = nullptr;
}

const geom::LinearRing* LinearRing::geometry() 
{
    return dynamic_cast<geom::LinearRing*>(geometry_);
}

void LinearRing::setCoordinates(std::vector<geom::Coordinate>& coordinates)
{
    auto sequence = geom::CoordinateArraySequenceFactory::instance()->create(&coordinates);
    dynamic_cast<geom::LinearRing*>(geometry_)->setPoints(sequence);
}

LinearRing::Ptr LinearRing::clone() const 
{
    // raw clone is virtual! :)
    return LinearRing::Ptr(raw_clone());
}

LinearRing* LinearRing::raw_clone() const
{
    LinearRing* newInstance = new LinearRing();
    // set the same reference frame
    newInstance->setCS(this->getCS());

    // copy the geometry
    newInstance->geometry_ = geometry_->clone(); 
    
    return newInstance;
}

}}
