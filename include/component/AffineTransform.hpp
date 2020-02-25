#ifndef SEMPR_COMPONENT_AFFINETRANSFORM_HPP_
#define SEMPR_COMPONENT_AFFINETRANSFORM_HPP_

#include <Eigen/Eigen>
#include "../Component.hpp"

namespace sempr {

/**
    The AffineTransform class is a simple wrapper for an Eigen::Affine3d
    instance. The magic comes and falls with builtins that allow us to work
    with these in the rules, and apply them to geometries etc.
*/
class AffineTransform : public Component {
    Eigen::Affine3d transform_;
public:
    using Ptr = std::shared_ptr<AffineTransform>;

    /// create identity
    AffineTransform();
    /// copy given transform
    AffineTransform(const Eigen::Affine3d& transform);

    /**
        Return the internally stored Eigen::Affine3d (by value)
    */
    Eigen::Affine3d transform() const;

    /**
        Sets a new transform
    */
    void setTransform(const Eigen::Affine3d& transform);
};

template <>
struct ComponentName<AffineTransform> {
    static constexpr const char* value = "Transform";
};

}

// register a to_string function to display AffineTransform in the rete network
#include <rete-core/Util.hpp>
namespace rete { namespace util {

template <> std::string to_string(const sempr::AffineTransform& c);
template <> std::string to_string(const sempr::AffineTransform::Ptr& c);

}}


#endif /* include guard: SEMPR_COMPONENT_AFFINETRANSFORM_HPP_ */

