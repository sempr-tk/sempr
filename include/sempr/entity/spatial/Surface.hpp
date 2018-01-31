#ifndef SEMPR_ENTITY_SPATIAL_SURFACE_HPP_
#define SEMPR_ENTITY_SPATIAL_SURFACE_HPP_

#include <sempr/entity/spatial/Geometry.hpp>

namespace sempr { namespace entity {

/**
    Container class for OGRSurface
*/
#pragma db object
class Surface : public Geometry {
public:
    using Ptr = std::shared_ptr<Surface>;
    ENTITY_DEFAULT_EVENT_METHODS(Surface, Geometry);

    Surface();
    Surface(const core::IDGenBase*);
    virtual ~Surface();

    OGRSurface* geometry() override;

private:
    friend class odb::access;
    // #pragma db type("TEXT")
    // OGRSurface* geometry_;   // OGRSurface is abstract!
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_SURFACE_HPP_ */
