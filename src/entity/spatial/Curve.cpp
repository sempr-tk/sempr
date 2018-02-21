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

Curve::Ptr Curve::clone() const {
    // raw clone is virtual! :)
    return Curve::Ptr(raw_clone());
}

Curve* Curve::raw_clone() const { return NULL; }

OGRCurve* Curve::geometry() {
    return NULL;
}

}}
