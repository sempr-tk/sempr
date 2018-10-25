#ifndef SEMPR_ENTITY_GEOMETRICOBJECT_H_
#define SEMPR_ENTITY_GEOMETRICOBJECT_H_

#include <sempr/entity/Entity.hpp>
#include <sempr/entity/spatial/Geometry.hpp>
#include <sempr/entity/SemanticEntity.hpp>
#include <sempr/core/RDF.hpp>
#include <odb/core.hxx>

#include <string>

namespace sempr { namespace entity {


#pragma db object
class GeometricObject : public SemanticEntity {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<GeometricObject>;

    GeometricObject();
    GeometricObject(bool temporary);
    GeometricObject(const core::IDGenBase*, bool temporary = false);

    const Geometry::Ptr geometry() const { return geometry_; }
    void geometry(const Geometry::Ptr geometry);    // takes the ownership of the given geometry entity

    const std::string type() const { return type_; }
    void type(const std::string& type);

    void registerProperty(const std::string& predicate, const std::string& object);

    // Gives all related object by a known predicate. (could be empty for all related objects)
    std::set<std::string> related(const std::string& predicate = "");

private:
    friend class odb::access;

    Geometry::Ptr geometry_;

    std::string type_;
};

}}


#endif /* end of include guard: SEMPR_ENTITY_GEOMETRICOBJECT_H_ */
