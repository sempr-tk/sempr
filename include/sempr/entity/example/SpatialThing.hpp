#ifndef SEMPR_ENTITY_SPATIALTHING_H_
#define SEMPR_ENTITY_SPATIALTHING_H_

#include <sempr/entity/Entity.hpp>
#include <sempr/entity/spatial/Polygon.hpp>
#include <odb/core.hxx>

#include <string>

namespace sempr { namespace entity {


#pragma db object
class SpatialThing : public Entity {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<SpatialThing>;

    SpatialThing();
    SpatialThing(const core::IDGenBase*);

    Polygon::Ptr& geometry() { return polygon_; }

    std::string& type() { return type_; }

protected:
    friend class odb::access;

    std::string type_;
    Polygon::Ptr polygon_;
};

}}


#endif /* end of include guard: SEMPR_ENTITY_SPATIALTHING_H_ */
