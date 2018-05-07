#ifndef SEMPR_ENTITY_SPATIAL_SURFACE_HPP_
#define SEMPR_ENTITY_SPATIAL_SURFACE_HPP_

#include <sempr/entity/spatial/Geometry.hpp>

namespace sempr { namespace entity {

/**
    Container class for OGRSurface
*/
#pragma db object
class Surface : public Geometry {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Surface>;

    Surface();
    Surface(const core::IDGenBase*);
    virtual ~Surface();

    OGRSurface* geometry() override;

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    Surface::Ptr clone() const;

private:
    friend class odb::access;
    // #pragma db type("TEXT")
    // OGRSurface* geometry_;   // OGRSurface is abstract!

    Surface* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_SURFACE_HPP_ */
