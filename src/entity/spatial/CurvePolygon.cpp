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

CurvePolygon::Ptr CurvePolygon::clone() const {
    // raw clone is virtual! :)
    return CurvePolygon::Ptr(raw_clone());
}

CurvePolygon* CurvePolygon::raw_clone() const
{
    CurvePolygon* newInstance = new CurvePolygon();
    // set the same reference frame
    newInstance->setCS(this->getCS());

    // copy the geometry
    *(newInstance->geometry_) = *geometry_; // use OGRGeometry copy ctor
    return newInstance;
}

OGRCurvePolygon* CurvePolygon::geometry() {
    return geometry_;
}

}}
