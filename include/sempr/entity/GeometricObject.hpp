#ifndef SEMPR_ENTITY_GEOMETRICOBJECT_H_
#define SEMPR_ENTITY_GEOMETRICOBJECT_H_

#include <sempr/entity/Entity.hpp>
#include <sempr/entity/spatial/Geometry.hpp>
#include <sempr/entity/SemanticEntity.hpp>
#include <odb/core.hxx>

#include <string>

namespace sempr { namespace entity {


#pragma db object
class GeometricObject : public SemanticEntity {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<GeometricObject>;

    GeometricObject();  //ToDo Allow temporary!!
    GeometricObject(const core::IDGenBase*);

    const Geometry::Ptr geometry() const { return geometry_; }
    void geometry(const Geometry::Ptr geometry);

    const std::string type() const { return type_; }
    //void type(const std::string& type) { updateProperty(type_, "rdf:type", type); }

    //void registerProperty(const std::string& predicate, const std::string& object);

    // Gives all related object by a known predicate. (could be empty for all related objects)
    //std::vector<std::string> related(const std::string& predicate = "");

private:
    friend class odb::access;

    Geometry::Ptr geometry_;

    std::string type_;
/*
    template<class T>
    void updateProperty(T& ref, const std::string predicate, const T& value)
    {
        removeProperty(predicate, value);   // will remove the first equal one!

        ref = value;

        registerProperty(predicate, value);

        changed();
    }*/
};

}}


#endif /* end of include guard: SEMPR_ENTITY_GEOMETRICOBJECT_H_ */
