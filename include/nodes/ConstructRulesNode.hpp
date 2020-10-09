#ifndef SEMPR_CONSTRUCTRULESNODE_HPP_
#define SEMPR_CONSTRUCTRULESNODE_HPP_

#include <rete-core/Production.hpp>
#include <rete-core/Accessors.hpp>

#include <map>

namespace sempr {
    class Core;

/**
    This node adds a bit of meta-reasoning, if you like: It is an effect that
    constructs new rules in the reasoner, given the usual string representation.

    It's main purpose is to treat rules just like data -- to be able to
    persist and edit them in the same manner as everything else.
*/
class ConstructRulesNode : public rete::Production {
    rete::PersistentInterpretation<std::string> rules_;
    Core* sempr_;

    // in order to deconstruct rules that have been removed we need to keep
    // track of which rules where inferred from which token
    std::map<rete::Token::Ptr, std::vector<size_t>> tokenRuleMapping_;
public:
    using Ptr = std::shared_ptr<ConstructRulesNode>;

    /**
        Creates a new ConstructRulesNode. This node will add the rules it gets
        through the given string accessor to the specified sempr instance.
    */
    ConstructRulesNode(Core* sempr, rete::PersistentInterpretation<std::string> rules);

    /**
        Constructs/deconstructs rules
    */
    void execute(rete::Token::Ptr, rete::PropagationFlag, std::vector<rete::WME::Ptr>&) override;

    std::string toString() const override;
};



}


#endif /* include guard: SEMPR_CONSTRUCTRULESNODE_HPP_ */
