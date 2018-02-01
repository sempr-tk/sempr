#include <sempr/entity/spatial/SpatialReference.hpp>
#include <SpatialReference_odb.h>

namespace sempr { namespace entity {

SpatialReference::SpatialReference()
    : SpatialReference(new core::IDGen<SpatialReference>())
{
}

SpatialReference::SpatialReference(const core::IDGenBase* idgen)
    : Entity(idgen)
{
}

SpatialReference::Ptr SpatialReference::getRoot()
{
    return SpatialReference::Ptr(NULL);
}

Eigen::Affine3d SpatialReference::transformationToRoot() const
{
    return Eigen::Affine3d::Identity();
}

Eigen::Affine3d SpatialReference::transformationFromRoot() const
{
    return Eigen::Affine3d::Identity();
}

}}
