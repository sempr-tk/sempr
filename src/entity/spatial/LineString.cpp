#include <sempr/entity/spatial/LineString.hpp>
#include <LineString_odb.h>

namespace sempr { namespace entity {

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

}}
