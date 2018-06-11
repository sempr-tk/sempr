#ifndef SEMPR_ENTITY_SPATIAL_LOCALCS_HPP_
#define SEMPR_ENTITY_SPATIAL_LOCALCS_HPP_

#include <sempr/entity/spatial/SpatialReference.hpp>
#include <sempr/entity/spatial/ProjectionCS.hpp>


namespace sempr { namespace entity {

/**
    A local coordinate system. Defines an affine transformation to its parent system, which is
    either another LocalCS or a ProjectionCS.
*/
#pragma db object
class LocalCS : public SpatialReference {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<LocalCS>;

    LocalCS();
    LocalCS(const core::IDGenBase*);

    SpatialReference::Ptr getRoot() override;
    bool isChildOf(SpatialReference::Ptr other) const override;
    Eigen::Affine3d transformationToRoot() const override;
    Eigen::Affine3d transformationFromRoot() const override;

    /**
        A helper method: Compute the inverse if the transformation by exploiting the fact that
        the matrix represents an affine transformation.
        BEWARE: This assumes that the transformation is not only affine, but also that no scaling
        or skewing appears -- so only rotation and translation is allowed!
        (Thats why there is no direct access to the Eigen::Affine3d, only through set[Rotation/Translation].)
    */
    static Eigen::Affine3d getInverseRotTrans(const Eigen::Affine3d& mat);

    // set/get the parent
    SpatialReference::Ptr getParent() const;
    void setParent(LocalCS::Ptr);
    void setParent(ProjectionCS::Ptr);


    /**
        Set the rotation-part of the transformation by specifying a rotation axis and an angle.
        These are not (x,y,z,w) of the quaternion! Internally, a quaternion will be constructed
        by normalizing the (x,y,z)-vector and using it together with Eigen::AngleAxisd.

        Maybe this is not totally standard-conform, but it helps me with debugging. Don't like it?
        Use the quaternion-version! :)
    */
    void setRotation(double x, double y, double z, double angle);
    /// set the rotation-part of the transformation by specifying a quaternion
    void setRotation(const Eigen::Quaterniond& quat);

    /// set the translation-part of the transformation
    void setTranslation(double x, double y, double z);
    void setTranslation(const Eigen::Vector3d& trans);


    // get the transformation to the parent
    Eigen::Affine3d getTransform() const;
    // set the transformation to the parent
    void setTransform(const Eigen::Affine3d& tf);
private:
    friend class odb::access;
    SpatialReference::Ptr parent_;
    #pragma db type("TEXT")
    Eigen::Affine3d transform_;
};

}}
#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LOCALCS_HPP_ */
