#include "nodes/ConstructRulesBuilder.hpp"
#include "nodes/ConstructRulesNode.hpp"

#include <rete-reasoner/Exceptions.hpp>

namespace sempr {

ConstructRulesBuilder::ConstructRulesBuilder(Core* sempr)
    : rete::NodeBuilder(
            "constructRules",
            rete::NodeBuilder::BuilderType::EFFECT),
      sempr_(sempr)
{
}


rete::Production::Ptr ConstructRulesBuilder::buildEffect(rete::ArgumentList& args) const
{
    // must have exactly one argument, a string.
    if (args.size() != 1) throw rete::NodeBuilderException("Invalid number of arguments (!= 1).");

    std::unique_ptr<rete::StringAccessor> rules;
    if (args[0].isConst())
    {
        rules.reset(new rete::ConstantStringAccessor(args[0].getAST()));
        rules->index() = 0;
    }
    else
    {
        if (!args[0].getAccessor()->canAs<rete::StringAccessor>())
        {
            throw rete::NodeBuilderException("Argument " + args[0].getVariableName() + " is not compatible with StringAccessor.");
        }

        rules.reset(args[0].getAccessor()->clone()->as<rete::StringAccessor>());
    }

    // build the node
    auto node = std::make_shared<ConstructRulesNode>(sempr_, std::move(rules));
    return node;
}


}
