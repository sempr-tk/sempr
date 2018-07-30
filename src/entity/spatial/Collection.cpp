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
    return this->geometry();
}

geom::GeometryCollection* Collection::geometry()
{
    return NULL; // have to be override by childs!
}

}}
