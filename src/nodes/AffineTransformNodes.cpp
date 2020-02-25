#include <rete-core/TupleWME.hpp>

#include "nodes/AffineTransformNodes.hpp"
#include "component/AffineTransform.hpp"


namespace sempr {

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
AffineTransformCreate::AffineTransformCreate(
        std::unique_ptr<rete::NumberAccessor> x,
        std::unique_ptr<rete::NumberAccessor> y,
        std::unique_ptr<rete::NumberAccessor> z,
        std::unique_ptr<rete::NumberAccessor> qx,
        std::unique_ptr<rete::NumberAccessor> qy,
        std::unique_ptr<rete::NumberAccessor> qz,
        std::unique_ptr<rete::NumberAccessor> qw
    )
    : rete::Builtin("tf:create"),
        x_(std::move(x)),
        y_(std::move(y)),
        z_(std::move(z)),
        qx_(std::move(qx)),
        qy_(std::move(qy)),
        qz_(std::move(qz)),
        qw_(std::move(qw))
{
}

rete::WME::Ptr AffineTransformCreate::process(rete::Token::Ptr token)
{
    Eigen::Vector3d translation(
        x_->getDouble(token),
        y_->getDouble(token),
        z_->getDouble(token)
    );

    Eigen::Quaterniond quaternion(
            qw_->getDouble(token),
            qx_->getDouble(token),
            qy_->getDouble(token),
            qz_->getDouble(token)
    );

    // create the eigen matrix representing the transformation
    Eigen::Affine3d tf = Eigen::Affine3d::Identity();
    tf.linear() = quaternion.toRotationMatrix();
    tf.translation() = translation;

    // pack it into a component, so that all builtins working with such
    // transformations can access it.
    auto component = std::make_shared<AffineTransform>(tf);

    // pack the component into a WME, as that is needed by the rete network.
    // Components that are part of entities in sempr are added to the rete as
    // part of a ECWME, but that's not correct here, as ECWME describes an
    // entity-component-*pair*. Hence, just use a TupleWME.
    //
    auto wme = std::make_shared<rete::TupleWME<AffineTransform::Ptr>>(component);
    return wme;
}

bool AffineTransformCreate::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const AffineTransformCreate*>(&other);
    if (!o) return false;

    return *(o->x_) == *(this->x_) &&
           *(o->y_) == *(this->y_) &&
           *(o->z_) == *(this->z_) &&
           *(o->qx_) == *(this->qx_) &&
           *(o->qy_) == *(this->qy_) &&
           *(o->qz_) == *(this->qz_) &&
           *(o->qw_) == *(this->qw_);
}

// ----------------------------------------------------------------------------
// Multiply
// ----------------------------------------------------------------------------
AffineTransformMul::AffineTransformMul(
    std::unique_ptr<rete::SpecificTypeAccessor<AffineTransform::Ptr>> left,
    std::unique_ptr<rete::SpecificTypeAccessor<AffineTransform::Ptr>> right
    )
    : rete::Builtin("tf:mul"),
        left_(std::move(left)),
        right_(std::move(right))
{
}

rete::WME::Ptr AffineTransformMul::process(rete::Token::Ptr token)
{
    // get the operands
    AffineTransform::Ptr left, right;
    left_->getValue(token, left);
    right_->getValue(token, right);

    // multiply transformations
    auto result = left->transform() * right->transform();

    // pack into result
    auto component = std::make_shared<AffineTransform>(result);
    auto wme = std::make_shared<rete::TupleWME<AffineTransform::Ptr>>(component);
    return wme;
}

bool AffineTransformMul::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const AffineTransformMul*>(&other);
    if (!o) return false;

    return *(this->left_) == *(o->left_) &&
           *(this->right_) == *(o->right_);
}


// ----------------------------------------------------------------------------
// Inverse
// ----------------------------------------------------------------------------
AffineTransformInv::AffineTransformInv(
    std::unique_ptr<rete::SpecificTypeAccessor<AffineTransform::Ptr>> tf
    )
    : rete::Builtin("tf:inv"),
        tf_(std::move(tf))
{
}


rete::WME::Ptr AffineTransformInv::process(rete::Token::Ptr token)
{
    // get operand
    AffineTransform::Ptr tf;
    tf_->getValue(token, tf);

    // compute inverse
    auto inv = tf->transform().inverse();

    // pack into result (wme)
    auto component = std::make_shared<AffineTransform>(inv);
    auto wme = std::make_shared<rete::TupleWME<AffineTransform::Ptr>>(component);
    return wme;
}

bool AffineTransformInv::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const AffineTransformInv*>(&other);
    if (!o) return false;

    return *(this->tf_) == *(o->tf_);
}

}
