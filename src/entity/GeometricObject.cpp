#include <sempr/entity/GeometricObject.hpp>
#include <GeometricObject_odb.h>


namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeometricObject)

GeometricObject::GeometricObject() :
    GeometricObject(new core::IDGen<GeometricObject>())
{
}

GeometricObject::GeometricObject(bool temporary) :
    GeometricObject(new core::IDGen<GeometricObject>(), temporary)
{
}

GeometricObject::GeometricObject(const core::IDGenBase* idgen, bool temporary) : 
    SemanticEntity(idgen, temporary)
{
    setDiscriminator<GeometricObject>();

    SemanticEntity::registerProperty(sempr::buildURI("subClassOf", sempr::core::rdfs::baseURI()) , "<http://www.opengis.net/ont/geosparql#SpatialObject>");
}

void GeometricObject::geometry(const Geometry::Ptr geometry)
{
    if (geometry_)
        SemanticEntity::removeProperty(sempr::buildURI(geometry_->id(), sempr::baseURI()), sempr::buildURI("subClassOf", sempr::core::rdfs::baseURI()), "<http://www.opengis.net/ont/geosparql#Geometry>");

    geometry_ = geometry;

    if (geometry_)  // cover nullptr set case
        SemanticEntity::registerProperty(sempr::buildURI(geometry_->id(), sempr::baseURI()), sempr::buildURI("subClassOf", sempr::core::rdfs::baseURI()), "<http://www.opengis.net/ont/geosparql#Geometry>");
}

void GeometricObject::type(const std::string& type)
{
    SemanticEntity::removeProperty(sempr::buildURI("type", sempr::core::rdf::baseURI()), type_);   // will remove the first equal one!

    type_ = type;

    SemanticEntity::registerProperty(sempr::buildURI("type", sempr::core::rdf::baseURI()), type_);
}

void GeometricObject::registerProperty(const std::string& predicate, const std::string& object)
{
    std::string obj = object;   // needed! if you try to passthrough object directly you will receive a compiler error based on type missmatch of const&!
    SemanticEntity::registerProperty(predicate, obj);
}

std::set<std::string> GeometricObject::related(const std::string& predicate)
{
    std::set<std::string> objects;
    for(auto t : *this)
    {   
        // empty predicate will add all related object to the result list
        if ( predicate.empty() || t.predicate == predicate)
            objects.insert(t.object);
    }
    return objects;
}

} /* entity */
} /* sempr */
