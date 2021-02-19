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
    if (args.size() != 2 && args.size() != 3)
        throw rete::NodeBuilderException("Wrong number of arguments (!= 2/3)");
    else if (args[0].isConst() ||
            !args[0].getAccessor()->getInterpretation<Entity::Ptr>())
        throw rete::NodeBuilderException("First argument must be bound to an Entity.");
    else if (args[1].isConst() ||
            !args[1].getAccessor()->getInterpretation<Component::Ptr>())
        throw rete::NodeBuilderException("Second argument must be bound to a Component.");
    else if (args.size() == 3 &&
            (
            (args[2].isConst() && !args[2].getAST().isString()) ||
            (args[2].isVariable() && !args[2].getAccessor()->getInterpretation<std::string>())
            ))
        throw rete::NodeBuilderException("Third argument must be a string.");

    // clone the accessors
    rete::PersistentInterpretation<Entity::Ptr>
        entity(args[0].getAccessor()->getInterpretation<Entity::Ptr>()
                                    ->makePersistent());

    rete::PersistentInterpretation<Component::Ptr>
        component(args[1].getAccessor()->getInterpretation<Component::Ptr>()
                                       ->makePersistent());

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
        rete::AccessorBase::Ptr tagAcc;
        if (args[2].isConst())
        {
            tagAcc = std::make_shared<rete::ConstantAccessor<std::string>>(
                        args[2].getAST().toString());
            tagAcc->index() = 0;
        }
        else
        {
            tagAcc = args[2].getAccessor();
        }

        node = std::make_shared<InferECNode>(
                std::move(entity),
                std::move(component),
                tagAcc->getInterpretation<std::string>()->makePersistent());
    }

    return node;
}

}
