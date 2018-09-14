#include <sempr/entity/spatial/reference/LocalCS.hpp>
#include <LocalCS_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(LocalCS)

LocalCS::LocalCS()
    : LocalCS(new core::IDGen<LocalCS>())
{
}

LocalCS::LocalCS(const core::IDGenBase* idgen)
    : SpatialReference(idgen)
{
    this->transform_.setIdentity();
    this->setDiscriminator<LocalCS>();
}

bool LocalCS::isEqual(const SpatialReference::Ptr other) const
{
    auto otherLocal = std::dynamic_pointer_cast<LocalCS>(other);

    if(otherLocal)
    {   // check if origin is equal:
        return parent_->isEqual(otherLocal->parent_) && transform_.isApprox(otherLocal->transform_);
    }
    else
    {
        return false;
    }
}

SpatialReference::Ptr LocalCS::getRoot()
{
    if (parent_) return parent_->getRoot(); // recursive definition.
    // if there is no parent, this is the root.
    return this->shared_from_base<SpatialReference>();
}

bool LocalCS::isChildOf(SpatialReference::Ptr other) const
{
    // I'd generally prefer an iterative implementation, but SpatialReference has no "getParent",
    // and I don't think there would be a noticable difference anyway. I don't expect the chain
    // of LocalCS's to be too long.
    if (!parent_) return false;
    return (parent_ == other) || parent_->isChildOf(other);
}

Eigen::Affine3d LocalCS::transformationToRoot() const
{
    //  root <-- A <-- B <-- C and geo_in_C
    // geo_in_B = C*geo_in_C
    // geo_in_A = B*geo_in_B
    // geo_in_root = A*geo_in_A = A*B*geo_in_B = A*B*C*geo_in_C
    if (!parent_) return Eigen::Affine3d::Identity();
    return parent_->transformationToRoot() * transform_; // (A*B) * C
}

Eigen::Affine3d LocalCS::transformationFromRoot() const
{
    //  root <-- A <-- B <-- C and geo_in_root
    // geo_in_A = A^-1 * geo_in_root
    // geo_in_B = B^-1 * geo_in_A
    // geo_in_C = C^-1 * geo_in_B = C^-1 * B^-1 * A^-1 * geo_in_root
    //                            = (A*B*C)^-1 * geo_in_root
    // Because (A*B*C)^-1 * geo_in_root = (A*B*C)^-1 * (A*B*C) * geo_in_C = geo_in_C

    if (!parent_) return Eigen::Affine3d::Identity();
    Eigen::Affine3d toRoot = transformationToRoot();
    Eigen::Affine3d fromRoot = LocalCS::getInverseRotTrans(toRoot);
    return fromRoot;
}


Eigen::Affine3d LocalCS::getInverseRotTrans(const Eigen::Affine3d &mat)
{
    // http://negativeprobability.blogspot.de/2011/11/affine-transformations-and-their.html
    /*
            [ R | T ]
        M = [ --+-- ] = (short) [R | T]
            [ 0 | 1 ]

        x' = M * x = R*x + T
        x' - T = R*x
        R^{-1} * (x'-T) = x
        R^{-1} * x - (R^{-1}*T) = x
        =>
        M^{-1} = [R^{-1} | -R^{-1}*T]
    */
    auto rot = mat.rotation();      // R
    auto rotInv = rot.transpose();  // R^{-1} (= R^t in case of a rotation matrix)
    auto trans = mat.translation(); // T
    Eigen::Affine3d inv;
    inv.setIdentity();
    inv.prerotate(rotInv);        // [R^{-1} | 0]
    inv.pretranslate(-1*rotInv*trans);  // [R^{-1} | -R^{-1}*T]

    return inv;
}

void LocalCS::setRotation(double x, double y, double z, double angle)
{
    Eigen::Vector3d axis(x, y, z);
    axis.normalize();
    Eigen::AngleAxisd aa(angle, axis);

    transform_.linear() = aa.toRotationMatrix();
}

void LocalCS::setRotation(const Eigen::Quaterniond& quat)
{
    transform_.linear() = quat.toRotationMatrix();
}

void LocalCS::setTranslation(double x, double y, double z)
{
    Eigen::Vector3d trans(x, y, z);
    setTranslation(trans);
}

void LocalCS::setTranslation(const Eigen::Vector3d& trans)
{
    transform_.translation() = trans;
}

SpatialReference::Ptr LocalCS::getParent() const
{
    return parent_;
}

void LocalCS::setParent(LocalCS::Ptr other)
{
    parent_ = other;
}

void LocalCS::setParent(ProjectionCS::Ptr other)
{
    parent_ = other;
}

void LocalCS::setParent(GeocentricCS::Ptr other)
{
    parent_ = other;
}

Eigen::Affine3d LocalCS::getTransform() const
{
    return transform_;
}


void LocalCS::setTransform(const Eigen::Affine3d& tf)
{
    transform_ = tf;
}

}}
