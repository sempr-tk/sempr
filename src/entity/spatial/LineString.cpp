#include <sempr/entity/spatial/LineString.hpp>
#include <LineString_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(LineString)

LineString::LineString() : LineString(new core::IDGen<LineString>())
{
}

LineString::LineString(const core::IDGenBase* idgen)
    : SimpleCurve(idgen)
{
    this->setDiscriminator<LineString>();
    geometry_ = static_cast<OGRLineString*>(OGRGeometryFactory::createGeometry(wkbLineString));
}

LineString::~LineString()
{
    OGRGeometryFactory::destroyGeometry(geometry_);
}

OGRLineString* LineString::geometry() {
    return geometry_;
}

LineString::Ptr LineString::clone() const {
    // raw clone is virtual! :)
    return LineString::Ptr(raw_clone());
}

LineString* LineString::raw_clone() const
{
    LineString* newInstance = new LineString();
    // set the same reference frame
    newInstance->setCS(this->getCS());

    // copy the geometry
    *(newInstance->geometry_) = *geometry_; // use OGRGeometry copy ctor
    return newInstance;
}

}}
