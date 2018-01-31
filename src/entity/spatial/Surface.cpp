#include <sempr/entity/spatial/Surface.hpp>
#include <Surface_odb.h>

namespace sempr { namespace entity {

Surface::Surface() : Surface(new core::IDGen<Surface>())
{
}

Surface::Surface(const core::IDGenBase* idgen)
    : Geometry(idgen)
{
    this->setDiscriminator<Surface>();
}

Surface::~Surface()
{
}

OGRSurface* Surface::geometry() {
    // to be overridden by concrete surfaces.
    return NULL;
}

}}
