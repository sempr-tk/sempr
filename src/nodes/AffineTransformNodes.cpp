#include <rete-core/TupleWME.hpp>

#include "nodes/AffineTransformNodes.hpp"
#include "component/AffineTransform.hpp"


namespace sempr {

// ----------------------------------------------------------------------------
// Create
// ----------------------------------------------------------------------------
AffineTransformCreate::AffineTransformCreate(
        rete::PersistentInterpretation<float> x,
        rete::PersistentInterpretation<float> y,
        rete::PersistentInterpretation<float> z,
        rete::PersistentInterpretation<float> qx,
        rete::PersistentInterpretation<float> qy,
        rete::PersistentInterpretation<float> qz,
        rete::PersistentInterpretation<float> qw
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
    float x, y, z;
    x_.interpretation->getValue(token, x);
    y_.interpretation->getValue(token, y);
    z_.interpretation->getValue(token, z);

    Eigen::Vector3d translation(x, y, z);

    float qx, qy, qz, qw;
    qx_.interpretation->getValue(token, qx);
    qy_.interpretation->getValue(token, qy);
    qz_.interpretation->getValue(token, qz);
    qw_.interpretation->getValue(token, qw);

    Eigen::Quaterniond quaternion(qw, qx, qy, qz);

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

    return *(o->x_.accessor) == *(this->x_.accessor) &&
           *(o->y_.accessor) == *(this->y_.accessor) &&
           *(o->z_.accessor) == *(this->z_.accessor) &&
           *(o->qx_.accessor) == *(this->qx_.accessor) &&
           *(o->qy_.accessor) == *(this->qy_.accessor) &&
           *(o->qz_.accessor) == *(this->qz_.accessor) &&
           *(o->qw_.accessor) == *(this->qw_.accessor);
}

// ----------------------------------------------------------------------------
// Get (extract parameters)
// ----------------------------------------------------------------------------
AffineTransformGet::AffineTransformGet(
    rete::PersistentInterpretation<AffineTransform::Ptr> tf
    )
    : rete::Builtin("tf:get"),
        tf_(std::move(tf))
{
}

rete::WME::Ptr AffineTransformGet::process(rete::Token::Ptr token)
{
    // get AffineTransform from token
    AffineTransform::Ptr tf;
    tf_.interpretation->getValue(token, tf);

    // compute translation and quaternion
    auto t = tf->transform().translation();
    Eigen::Quaterniond rot(tf->transform().rotation());

    // put parameters into tuplewme
    auto wme = std::make_shared<
                rete::TupleWME<double, double, double,
                               double, double, double, double>>(
        t.x(), t.y(), t.z(),
        rot.x(), rot.y(), rot.z(), rot.w()
    );

    return wme;
}

bool AffineTransformGet::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const AffineTransformGet*>(&other);
    if (!o) return false;

    return *(this->tf_.accessor) == *(o->tf_.accessor);
}

// ----------------------------------------------------------------------------
// Multiply
// ----------------------------------------------------------------------------
AffineTransformMul::AffineTransformMul(
    rete::PersistentInterpretation<AffineTransform::Ptr> left,
    rete::PersistentInterpretation<AffineTransform::Ptr> right
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
    left_.interpretation->getValue(token, left);
    right_.interpretation->getValue(token, right);

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

    return *(this->left_.accessor) == *(o->left_.accessor) &&
           *(this->right_.accessor) == *(o->right_.accessor);
}


// ----------------------------------------------------------------------------
// Inverse
// ----------------------------------------------------------------------------
AffineTransformInv::AffineTransformInv(
    rete::PersistentInterpretation<AffineTransform::Ptr> tf
    )
    : rete::Builtin("tf:inv"),
        tf_(std::move(tf))
{
}


rete::WME::Ptr AffineTransformInv::process(rete::Token::Ptr token)
{
    // get operand
    AffineTransform::Ptr tf;
    tf_.interpretation->getValue(token, tf);

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

    return *(this->tf_.accessor) == *(o->tf_.accessor);
}

}
