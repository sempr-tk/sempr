#include <sempr/entity/spatial/LocalCS.hpp>
#include <LocalCS_odb.h>

namespace sempr { namespace entity {

LocalCS::LocalCS()
    : SpatialReference(new core::IDGen<LocalCS>())
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


}}
