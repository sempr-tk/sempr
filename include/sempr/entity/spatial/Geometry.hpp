#ifndef SEMPR_ENTITY_SPATIAL_GEOMETRY_HPP_
#define SEMPR_ENTITY_SPATIAL_GEOMETRY_HPP_

#include <sempr/entity/Entity.hpp>
#include <ogr_geometry.h>



namespace sempr { namespace entity {

/**
    A base class for all geometry entities. Contains methods to manage spatial reference systems
    but no concrete geometry -- just a pure virtual OGRGeometry* geometry() method to be implemented
    by the derived classes (point, polygon, ...).
*/
#pragma db object
class Geometry : public Entity {
public:
    using Ptr = std::shared_ptr<Geometry>;
    ENTITY_DEFAULT_EVENT_METHODS(Geometry, Entity);

    Geometry();
    Geometry(const core::IDGenBase*);
    virtual ~Geometry();

    // Note: I'd like this to be a pure virtual, but in that case ODB does not create a
    // discriminator for the class (why should it, if no instances of it can be created?),
    // which again leads to compiler errors.
    virtual OGRGeometry* geometry() { return NULL; };

    // TODO spatial reference!

private:
    friend class odb::access;
};

}}


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEOMETRY_HPP_ */
