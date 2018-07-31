#include <sempr/entity/spatial/GeometryCollection.hpp>
#include <GeometryCollection_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeometryCollection)

GeometryCollection::GeometryCollection() : GeometryCollection(new core::IDGen<GeometryCollection>())
{
}

GeometryCollection::GeometryCollection(const core::IDGenBase* idgen)
    : Collection(idgen)
{
    this->setDiscriminator<GeometryCollection>();
    geometry_ = factory_->createGeometryCollection();
}

GeometryCollection::~GeometryCollection()
{
    if (geometry_)
    {
        factory_->destroyGeometry(geometry_);
        geometry_ = nullptr;
    }
}

const geom::GeometryCollection* GeometryCollection::getGeometry()
{
    return geometry_;
}

geom::GeometryCollection* GeometryCollection::getGeometry() const
{
    return geometry_;
}

void GeometryCollection::setGeometry(geom::GeometryCollection* geometry)
{
    if (geometry_)
        factory_->destroyGeometry(geometry_);

    geometry_ = geometry;
}

void GeometryCollection::setCollection(const std::vector<geom::Geometry*> geoms)
{
    setGeometry(factory_->createGeometryCollection(geoms));
}

GeometryCollection::Ptr GeometryCollection::clone() const 
{
    // raw clone is virtual! :)
    return GeometryCollection::Ptr(raw_clone());
}

GeometryCollection* GeometryCollection::raw_clone() const
{
    GeometryCollection* newInstance = new GeometryCollection();
    // set the same reference frame
    newInstance->setCS(this->getCS());

    // copy the geometry
    newInstance->setGeometry( dynamic_cast<geom::GeometryCollection*>(geometry_->clone()) );

    return newInstance;
}

}}
