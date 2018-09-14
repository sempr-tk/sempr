#include <sempr/entity/spatial/reference/GlobalCS.hpp>
#include <GlobalCS_odb.h>

#include <string>
#include <typeinfo>

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

SpatialReference::Ptr GlobalCS::getRoot()
{
    // GlobalCS is always a root
    return this->shared_from_base<SpatialReference>();
}

Eigen::Affine3d GlobalCS::transformationToRoot() const
{
    return Eigen::Affine3d::Identity();
}

Eigen::Affine3d GlobalCS::transformationFromRoot() const
{
    return Eigen::Affine3d::Identity();
}

FilterList GlobalCS::to(const GlobalCS::Ptr other) const
{
    //only for ODB support - not abstract
    assert(1);
    return FilterList();
}

FilterPtr GlobalCS::forward() const
{
    //only for ODB support - not abstract
    assert(1);
    return FilterPtr(nullptr);
}

FilterPtr GlobalCS::reverse() const
{
    //only for ODB support - not abstract
    assert(1);
    return FilterPtr(nullptr);
}



}}
