#include <sempr/entity/spatial/reference/SpatialReference.hpp>
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

bool SpatialReference::isEqual(const SpatialReference::Ptr other) const
 {
    // avoid expr. side effects warning:
    auto& tmpOther = *other;
    auto& tmpThis = *this;
    
    // type check for all childs without any attributes valid. Have to be override if child contain attributes.
    return typeid(tmpOther) == typeid(tmpThis);
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
