#include <sempr/entity/spatial/Geometry.hpp>
#include <Geometry_odb.h>

namespace sempr { namespace entity {

Geometry::Geometry() : Geometry(new core::IDGen<Geometry>())
{
}

Geometry::Geometry(const core::IDGenBase* idgen)
    : Entity(idgen)
{
    this->setDiscriminator<Geometry>();
}

Geometry::~Geometry()
{
}

}}
