#include <sempr/entity/spatial/Lineal.hpp>
#include <Lineal_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(Lineal)

Lineal::Lineal(const core::IDGenBase* idgen)
    : Geometry(idgen)
{
    this->setDiscriminator<Lineal>();
}

Lineal::Lineal()
{
}

Lineal::~Lineal()
{
}

const geom::Lineal* Lineal::getGeometry() const
{
    return nullptr;
}

geom::Lineal* Lineal::getGeometryMut()
{
    return nullptr; // have to be override by childs!
}

}}
