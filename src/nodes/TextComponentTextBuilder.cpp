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
    rete::util::requireNumberOfArgs(args, 2);
    rete::util::requireUnboundVariable(args, 0);

    auto text = rete::util::requireInterpretation<TextComponent::Ptr>(args, 1);

    // create the node
    auto node = std::make_shared<TextComponentTextNode>(std::move(text));

    // create an accessor for the result
    auto resultAcc = std::make_shared<rete::TupleWME<std::string>::Accessor<0>>();

    // bind the result
    args[0].bind(resultAcc);

    // return the node
    return node;
}


}
