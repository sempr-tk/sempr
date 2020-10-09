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
    SEMPR_COMPONENT

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


    /**
        Serialization with cereal
    */
    template <class Archive>
    void save(Archive& ar) const
    {
        double data[4][4];
        for (int col = 0; col < 4; col++)
        {
            for (int row = 0; row < 4; row++)
            {
                data[col][row] = transform_.matrix()(col, row);
            }
        }

        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("matrix", data) );
    }

    template <class Archive>
    void load(Archive& ar)
    {
        double data[4][4];
        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("matrix", data) );

        for (int col = 0; col < 4; col++)
        {
            for (int row = 0; row < 4; row++)
            {
                transform_.matrix()(col, row) = data[col][row];
            }
        }
    }
};

template <>
struct ComponentName<AffineTransform> {
    static constexpr const char* value = "Transform";
};

}


CEREAL_REGISTER_TYPE(sempr::AffineTransform)


// register a to_string function to display AffineTransform in the rete network
#include <rete-core/Util.hpp>
namespace rete { namespace util {

template <> std::string to_string(const sempr::AffineTransform& c);
template <> std::string to_string(const sempr::AffineTransform::Ptr& c);

}}


#endif /* include guard: SEMPR_COMPONENT_AFFINETRANSFORM_HPP_ */

