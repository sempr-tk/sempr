#include <sempr/entity/spatial/SimpleCurve.hpp>
#include <SimpleCurve_odb.h>

namespace sempr { namespace entity {

SimpleCurve::SimpleCurve() : SimpleCurve(new core::IDGen<SimpleCurve>())
{
}

SimpleCurve::SimpleCurve(const core::IDGenBase* idgen)
    : Curve(idgen)
{
    this->setDiscriminator<SimpleCurve>();
}

SimpleCurve::~SimpleCurve()
{
}

OGRSimpleCurve* SimpleCurve::geometry() {
    return NULL;
}

}}
