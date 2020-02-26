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


#include <sstream>
#include <iomanip>
namespace rete { namespace util {

template <>
std::string to_string(const sempr::AffineTransform& c)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);

    Eigen::Quaterniond q(c.transform().rotation());
    auto t = c.transform().translation();

    ss << "tf{ "
       << "x: " << t.x() << ", "
       << "y: " << t.y() << ", "
       << "z: " << t.z() << ", "
       << "qx: " << q.x() << ", "
       << "qy: " << q.y() << ", "
       << "qz: " << q.z() << ", "
       << "qw: " << q.w()
       << " }";
    return ss.str();
}

template<>
std::string to_string(const sempr::AffineTransform::Ptr& c)
{
    if (!c) return "tf{nullptr}";
    return to_string(*c);
}

}}
