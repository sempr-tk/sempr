#include <sempr/entity/spatial/LocalCS.hpp>
#include <LocalCS_odb.h>

namespace sempr { namespace entity {

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


SpatialReference::Ptr LocalCS::getRoot()
{
    if (parent_) return parent_->getRoot(); // recursive definition.
    // if there is no parent, this is the root.
    return this->shared_from_base<SpatialReference>();
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


}}
