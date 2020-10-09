#ifndef SEMPR_CONSTRUCTRULESBUILDER_HPP_
#define SEMPR_CONSTRUCTRULESBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

namespace sempr {
    class Core;

/**
    Constructs the ConstructRuleNodes.

    sempr:constructRule(?string)
*/
class ConstructRulesBuilder : public rete::NodeBuilder {
    Core* sempr_;
public:
    /**
        Takes a pointer to the sempr::Core instance that the rules will be
        created in.
    */
    ConstructRulesBuilder(Core* sempr);

    rete::Production::Ptr buildEffect(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_CONSTRUCTRULESBUILDER_HPP_ */
