#include "component/AffineTransform.hpp"

namespace sempr {

AffineTransform::AffineTransform()
    : transform_(Eigen::Affine3d::Identity())
{
}

AffineTransform::AffineTransform(const Eigen::Affine3d& t)
    : transform_(t)
{
}

Eigen::Affine3d AffineTransform::transform() const
{
    return transform_;
}

void AffineTransform::setTransform(const Eigen::Affine3d& t)
{
    transform_ = t;
}

}
