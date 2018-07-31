#include <sempr/entity/spatial/Collection.hpp>
#include <Collection_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(Collection)

Collection::Collection(const core::IDGenBase* idgen)
    : Geometry(idgen)
{
    this->setDiscriminator<Collection>();
}

Collection::Collection()
{
}

Collection::~Collection()
{
}

const geom::GeometryCollection* Collection::getGeometry()
{
    return nullptr;
}

geom::GeometryCollection* Collection::getGeometry() const
{
    return nullptr; // have to be override by childs!
}

}}
