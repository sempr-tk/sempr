#include <sempr/entity/spatial/Curve.hpp>
#include <Curve_odb.h>

namespace sempr { namespace entity {

Curve::Curve() : Curve(new core::IDGen<Curve>())
{
}

Curve::Curve(const core::IDGenBase* idgen)
    : Geometry(idgen)
{
    this->setDiscriminator<Curve>();
}

Curve::~Curve()
{
}

OGRCurve* Curve::geometry() {
    return NULL;
}

}}
