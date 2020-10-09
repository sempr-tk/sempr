#include "component/TextComponent.hpp"

namespace sempr {


TextComponent::TextComponent()
{
}

TextComponent::TextComponent(const std::string& text)
    : text_(text)
{
}

void TextComponent::setText(const std::string& text)
{
    text_ = text;
}

std::string TextComponent::text() const
{
    return text_;
}

}


namespace rete { namespace util {

template <>
std::string to_string(const sempr::TextComponent& /*c*/)
{
    return "TextComponent";
}

template <>
std::string to_string(const sempr::TextComponent::Ptr& c)
{
    if (!c) return "nullptr";
    return to_string(*c);
}


}}

