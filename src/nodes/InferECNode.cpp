#include "nodes/InferECNode.hpp"

namespace sempr {

InferECNode::InferECNode(
    std::unique_ptr<rete::SpecificTypeAccessor<Entity::Ptr>> entity,
    std::unique_ptr<rete::SpecificTypeAccessor<Component::Ptr>> component)
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
        entity_->getValue(token, e);
        component_->getValue(token, c);

        auto ecwme = std::make_shared<ECWME>(e, c);
        inferred.push_back(ecwme);
    }
}

std::string InferECNode::toString() const
{
    return "EC(" + entity_->toString() + ", " + component_->toString() + ")";
}


}
