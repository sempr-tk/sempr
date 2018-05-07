#include <sempr/entity/spatial/SpatialReference.hpp>
#include <SpatialReference_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(SpatialReference)

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

bool SpatialReference::isChildOf(SpatialReference::Ptr other) const
{
    // The base class has no knowledge of parent/child relations. That is a feature introduced
    // by LocalCS.
    return false;
}

}}
