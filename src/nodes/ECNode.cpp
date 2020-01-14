#include "nodes/ECNode.hpp"

namespace sempr {

ComponentTagNode::ComponentTagNode(const std::string& tag)
    : tag_(tag)
{
}

std::string ComponentTagNode::getDOTAttr() const
{
    return "[label=\"" + rete::util::dotEscape("Component.Tag == " + tag_) + "\"]";
}

void ComponentTagNode::activate(rete::WME::Ptr wme, rete::PropagationFlag flag)
{
    if (flag == rete::PropagationFlag::RETRACT)
    {
        // just pass it on
        propagate(wme, rete::PropagationFlag::RETRACT);
    }

    // actually check the tag:
    auto ecwme = std::static_pointer_cast<ECWME>(wme);
    auto component = std::get<1>(ecwme->value_);

    if (component->getTag() == tag_)
    {
        propagate(wme, flag);
    }
}

bool ComponentTagNode::operator==(const AlphaNode& other) const
{
    auto optr = dynamic_cast<const ComponentTagNode*>(&other);
    if (optr && optr->tag_ == this->tag_)
    {
        return true;
    }
    return false;
}

}
