#ifndef SEMPR_TEXTCOMPONENT_HPP_
#define SEMPR_TEXTCOMPONENT_HPP_

#include "../Component.hpp"

#include <string>

namespace sempr {

/**
    This class is just a component for string values.
*/
class TextComponent : public Component {
    SEMPR_COMPONENT

    std::string text_;
public:
    using Ptr = std::shared_ptr<TextComponent>;

    TextComponent();
    TextComponent(const std::string&);

    void setText(const std::string&);
    std::string text() const;

    /**
        Serialization with cereal
    */
    template <class Archive>
    void save(Archive& ar) const
    {
        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("text", text_) );
    }

    template <class Archive>
    void load(Archive& ar)
    {
        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("text", text_) );
    }
};

template <>
struct ComponentName<TextComponent> {
    static constexpr const char* value = "TextComponent";
};

}

CEREAL_REGISTER_TYPE(sempr::TextComponent)

// also, register a to_string function to display in a rete network
#include <rete-core/Util.hpp>

namespace rete { namespace util {

template <> std::string to_string(const sempr::TextComponent& c);
template <> std::string to_string(const sempr::TextComponent::Ptr& c);

}}

#endif /* include guard: SEMPR_TEXTCOMPONENT_HPP_ */
