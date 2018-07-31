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
    if (geometry_)
    {
        factory_->destroyGeometry(geometry_);
        geometry_ = nullptr;
    }
}

const geom::LinearRing* LinearRing::getGeometry()
{
    return geometry_;
}

geom::LinearRing* LinearRing::getGeometry() const
{
    return geometry_;
}

void LinearRing::setGeometry(geom::LinearRing* geometry)
{
    if (geometry_)
        factory_->destroyGeometry(geometry_);

    geometry_ = geometry;
}

void LinearRing::setCoordinates(std::vector<geom::Coordinate>& coordinates)
{
    auto sequence = geom::CoordinateArraySequenceFactory::instance()->create();
    sequence->setPoints(coordinates);
    geometry_->setPoints(sequence);
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
    newInstance->setGeometry( dynamic_cast<geom::LinearRing*>(geometry_->clone()) );
    
    return newInstance;
}

}}
