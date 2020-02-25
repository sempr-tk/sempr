#include <rete-core/TupleWME.hpp>

#include "nodes/AffineTransformNodes.hpp"
#include "component/AffineTransform.hpp"


namespace sempr {

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
    auto wme = std::make_shared<rete::TupleWME<Component::Ptr>>(component);
    return wme;
}

}
