#include "nodes/InferECNode.hpp"

namespace sempr {

InferECNode::InferECNode(
    rete::PersistentInterpretation<Entity::Ptr> entity,
    rete::PersistentInterpretation<Component::Ptr> component)
    : entity_(std::move(entity)), component_(std::move(component))
{
}

void InferECNode::execute(
        rete::Token::Ptr token, rete::PropagationFlag flag,
        std::vector<rete::WME::Ptr>& inferred)
{
    if (flag == rete::PropagationFlag::ASSERT || flag == rete::PropagationFlag::UPDATE)
    {
        Entity::Ptr e;
        Component::Ptr c;
        entity_.interpretation->getValue(token, e);
        component_.interpretation->getValue(token, c);

        auto ecwme = std::make_shared<ECWME>(e, c);
        inferred.push_back(ecwme);
    }
}

std::string InferECNode::toString() const
{
    return "EC(" + entity_.accessor->toString() + ", "
                 + component_.accessor->toString() + ")";
}


}
