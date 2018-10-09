#include <sempr/entity/GeometricObject.hpp>
#include <GeometricObject_odb.h>


namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeometricObject)

GeometricObject::GeometricObject(const core::IDGenBase* idgen)
    : SemanticEntity(idgen)
{
    setDiscriminator<GeometricObject>();

    registerProperty("rdfs:subClassOf", "<http://www.opengis.net/ont/geosparql#SpatialObject>");
}

GeometricObject::GeometricObject() :
    GeometricObject(new core::IDGen<GeometricObject>())
{
}

void GeometricObject::geometry(const Geometry::Ptr geometry)
{
    if (geometry_)
        removeProperty(geometry_->id(), "rdfs:subClassOf", "<http://www.opengis.net/ont/geosparql#Geometry>");

    geometry_ = geometry;

    if (geometry_)  // cover nullptr set case
        registerProperty(geometry_->id(), "rdfs:subClassOf", "<http://www.opengis.net/ont/geosparql#Geometry>");

    changed();
}

} /* entity */
} /* sempr */
