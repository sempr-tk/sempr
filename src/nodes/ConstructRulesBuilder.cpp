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
    rete::util::requireNumberOfArgs(args, 1);
    auto rules = rete::util::requireInterpretation<std::string>(args, 0);

    // build the node
    auto node = std::make_shared<ConstructRulesNode>(sempr_, std::move(rules));
    return node;
}


}
