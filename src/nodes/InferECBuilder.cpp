#include "nodes/InferECBuilder.hpp"

namespace sempr {

InferECBuilder::InferECBuilder()
    : rete::NodeBuilder("EC", rete::NodeBuilder::BuilderType::EFFECT)
{
}

rete::Production::Ptr InferECBuilder::buildEffect(rete::ArgumentList& args) const
{
    // needs exactly 2 or 3 args, which must be vars bound to an entity and a
    // component, and an optional tag
    rete::util::requireNumberOfArgs(args, 2, 3);

    auto entity = rete::util::requireInterpretation<Entity::Ptr>(args, 0);
    auto component = rete::util::requireInterpretation<Component::Ptr>(args, 1);

    InferECNode::Ptr node;
    if (args.size() == 2)
    {
        rete::ConstantAccessor<std::string> tag("");
        tag.index() = 0;

        // create the node
        node = std::make_shared<InferECNode>(
                std::move(entity),
                std::move(component),
                tag.getInterpretation<std::string>()->makePersistent());
    }
    else // args.size() == 3
    {
        auto tag = rete::util::requireInterpretation<std::string>(args, 2);
        node = std::make_shared<InferECNode>(
                std::move(entity),
                std::move(component),
                std::move(tag)
        );
    }

    return node;
}

}
