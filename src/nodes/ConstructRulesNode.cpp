#include "nodes/ConstructRulesNode.hpp"
#include "Core.hpp"

namespace sempr {

ConstructRulesNode::ConstructRulesNode(
        Core* sempr,
        rete::PersistentInterpretation<std::string> rules)
    :
        rules_(std::move(rules)), sempr_(sempr)
{
}

void ConstructRulesNode::execute(
        rete::Token::Ptr token,
        rete::PropagationFlag flag,
        std::vector<rete::WME::Ptr>& /*inferred*/)
{
    std::vector<size_t> inferredRules;

    if (flag == rete::PropagationFlag::ASSERT || flag == rete::PropagationFlag::UPDATE)
    {
        std::string rulestring;
        rules_.interpretation->getValue(token, rulestring);

        // create new rules
        inferredRules = sempr_->addRules(rulestring);
    }

    if (flag == rete::PropagationFlag::RETRACT || flag == rete::PropagationFlag::UPDATE)
    {
        // remove old rules
        auto toRemove = tokenRuleMapping_[token];
        for (size_t id : toRemove)
        {
            sempr_->removeRule(id);
        }
    }

    // overwrite the mapping -- either inferredRules is empty (RETRACT),
    // or it contains the new/updated rules
    tokenRuleMapping_[token] = inferredRules;
}

std::string ConstructRulesNode::toString() const
{
    return "ConstructRules(" + rules_.accessor->toString() + ")";
}

}
