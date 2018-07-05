#include <GlobalCS_odb.h>
#include <sempr/entity/spatial/GlobalCS.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GlobalCS)

GlobalCS::GlobalCS()
{
    // nothing to do.
}

GlobalCS::GlobalCS(const core::IDGenBase* idgen)
    : SpatialReference(idgen)
{
}

GlobalCS::~GlobalCS()
{
}

/*
std::shared_ptr<OGRCoordinateTransformation> GlobalCS::to(GlobalCS::Ptr other)
{
    auto raw = OGRCreateCoordinateTransformation(&frame_, &(other->frame_));
    auto t = std::shared_ptr<OGRCoordinateTransformation>(raw, OGRCoordinateTransformation::DestroyCT);
    return t;
}
 */


SpatialReference::Ptr GlobalCS::getRoot()
{
    // GlobalCS is always a root
    return this->shared_from_base<GlobalCS>();
}

Eigen::Affine3d GlobalCS::transformationToRoot() const
{
    return Eigen::Affine3d::Identity();
}

Eigen::Affine3d GlobalCS::transformationFromRoot() const
{
    return Eigen::Affine3d::Identity();
}



}}
