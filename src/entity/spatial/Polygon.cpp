#include <sempr/entity/spatial/Polygon.hpp>
#include <Polygon_odb.h>

namespace sempr { namespace entity {

Polygon::Polygon() : Polygon(new core::IDGen<Polygon>())
{
}

Polygon::Polygon(const core::IDGenBase* idgen)
    : CurvePolygon(idgen)
{
    this->setDiscriminator<Polygon>();
    geometry_ = static_cast<OGRPolygon*>(OGRGeometryFactory::createGeometry(wkbPolygon));
}

Polygon::~Polygon()
{
    OGRGeometryFactory::destroyGeometry(geometry_);
}

OGRPolygon* Polygon::geometry() {
    return geometry_;
}

}}
