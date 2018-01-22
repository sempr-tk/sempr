#include <sempr/entity/spatial/CurvePolygon.hpp>
#include <CurvePolygon_odb.h>

namespace sempr { namespace entity {

CurvePolygon::CurvePolygon() : CurvePolygon(new core::IDGen<CurvePolygon>())
{
}

CurvePolygon::CurvePolygon(const core::IDGenBase* idgen)
    : Geometry(idgen)
{
    this->setDiscriminator<CurvePolygon>();
    geometry_ = static_cast<OGRCurvePolygon*>(OGRGeometryFactory::createGeometry(wkbCurvePolygon));
}

CurvePolygon::~CurvePolygon()
{
    OGRGeometryFactory::destroyGeometry(geometry_);
}

OGRCurvePolygon* CurvePolygon::geometry() {
    return geometry_;
}

}}
