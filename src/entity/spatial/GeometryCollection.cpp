#include <sempr/entity/spatial/GeometryCollection.hpp>
#include <GeometryCollection_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeometryCollection)

GeometryCollection::GeometryCollection() : GeometryCollection(new core::IDGen<GeometryCollection>())
{
}

GeometryCollection::GeometryCollection(const core::IDGenBase* idgen)
    : Geometry(idgen)
{
    this->setDiscriminator<GeometryCollection>();
    geometry_ = factory_->createGeometryCollection();
}

GeometryCollection::~GeometryCollection()
{
    factory_->destroyGeometry(geometry_);
    geometry_ = nullptr;
}

geom::GeometryCollection* GeometryCollection::geometry() {
    return geometry_;
}

GeometryCollection::Ptr GeometryCollection::clone() const {
    // raw clone is virtual! :)
    return GeometryCollection::Ptr(raw_clone());
}

GeometryCollection* GeometryCollection::raw_clone() const
{
    GeometryCollection* newInstance = new GeometryCollection();
    // set the same reference frame
    newInstance->setCS(this->getCS());

    // copy the geometry
    newInstance->geometry_ = dynamic_cast<geom::GeometryCollection*>(geometry_->clone()); 

    return newInstance;
}

}}
