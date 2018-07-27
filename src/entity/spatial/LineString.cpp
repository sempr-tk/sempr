#include <sempr/entity/spatial/LineString.hpp>
#include <LineString_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(LineString)

LineString::LineString() : LineString(new core::IDGen<LineString>())
{
}

LineString::LineString(const core::IDGenBase* idgen)
    : Lineal(idgen)
{
    this->setDiscriminator<LineString>();
    geometry_ = factory_->createLineString();
}

LineString::~LineString()
{
    factory_->destroyGeometry(geometry_);
    geometry_ = nullptr;
}

const geom::LineString* LineString::geometry() 
{
    return dynamic_cast<geom::LineString*>(geometry_);
}

void LineString::setCoordinates(std::vector<geom::Coordinate>& coordinates)
{
    auto sequence = geom::CoordinateArraySequenceFactory::instance()->create();
    sequence->setPoints(coordinates);
    auto lineString = factory_->createLineString(sequence);
    setGeometry(lineString);
}

LineString::Ptr LineString::clone() const 
{
    // raw clone is virtual! :)
    return LineString::Ptr(raw_clone());
}

LineString* LineString::raw_clone() const
{
    LineString* newInstance = new LineString();
    // set the same reference frame
    newInstance->setCS(this->getCS());

    // copy the geometry
    newInstance->geometry_ = geometry_->clone(); 
    
    return newInstance;
}

}}
