#include <rete-core/TupleWME.hpp>

#include "nodes/TextComponentTextNode.hpp"

namespace sempr {

TextComponentTextNode::TextComponentTextNode(
        rete::PersistentInterpretation<TextComponent::Ptr> c)
    :
        rete::Builtin("text:value"),
        component_(std::move(c))
{
}

rete::WME::Ptr TextComponentTextNode::process(rete::Token::Ptr token)
{
    TextComponent::Ptr c;
    component_.interpretation->getValue(token, c);

    auto result = std::make_shared<rete::TupleWME<std::string>>(c->text());
    return result;
}

bool TextComponentTextNode::operator == (const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const TextComponentTextNode*>(&other);
    if (!o)
        return false;
    else
        return (*this->component_.accessor == *o->component_.accessor);
}


}
