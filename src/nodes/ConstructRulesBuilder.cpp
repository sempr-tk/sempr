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

    rete::PersistentInterpretation<std::string> rules;
    if (args[0].isConst())
    {
        auto acc = new rete::ConstantAccessor<std::string>(args[0].getAST());
        acc->index() = 0;
        rules = acc->getInterpretation<std::string>()->makePersistent();
    }
    else
    {
        if (!args[0].getAccessor()->getInterpretation<std::string>())
        {
            throw rete::NodeBuilderException("Argument " + args[0].getVariableName() + " has no interpretation as string.");
        }

        rules = args[0].getAccessor()->getInterpretation<std::string>()->makePersistent();
    }

    // build the node
    auto node = std::make_shared<ConstructRulesNode>(sempr_, std::move(rules));
    return node;
}


}
