#include <rete-reasoner/Exceptions.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/TupleWMEAccessor.hpp>

#include "nodes/TextComponentTextBuilder.hpp"
#include "nodes/TextComponentTextNode.hpp"

namespace sempr {

TextComponentTextBuilder::TextComponentTextBuilder()
    : rete::NodeBuilder("text:value", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr TextComponentTextBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 2 args: Unbound var for result, and a TextComponent
    if (args.size() != 2) throw rete::NodeBuilderException("Invalid number of arguments (!=2).");
    if (!args[0].isVariable() || args[0].getAccessor())
        throw rete::NodeBuilderException("First argument must be an unbound variable for the result.");

    if (!args[1].isVariable() || !args[1].getAccessor() ||
        !args[1].getAccessor()->canAs<rete::SpecificTypeAccessor<TextComponent::Ptr>>())
    {
        throw rete::NodeBuilderException("Second argument must be bound to a TextComponent.");
    }

    // clone the accessor
    std::unique_ptr<rete::SpecificTypeAccessor<TextComponent::Ptr>>
    text(
        args[1].getAccessor()->clone()->as<rete::SpecificTypeAccessor<TextComponent::Ptr>>()
    );

    // create the node
    auto node = std::make_shared<TextComponentTextNode>(std::move(text));

    // create an accessor for the result
    auto resultAcc = std::make_shared<rete::TupleWMEAccessor<0, rete::TupleWME<std::string>>>();

    // bind the result
    args[0].bind(resultAcc);

    // return the node
    return node;
}


}
