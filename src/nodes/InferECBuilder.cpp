#include "nodes/InferECBuilder.hpp"

namespace sempr {

InferECBuilder::InferECBuilder()
    : rete::NodeBuilder("EC", rete::NodeBuilder::BuilderType::EFFECT)
{
}

rete::Production::Ptr InferECBuilder::buildEffect(rete::ArgumentList& args) const
{
    // needs exactly 2 args, which must be vars bound to an entity and a
    // component
    if (args.size() != 2)
        throw rete::NodeBuilderException("Wrong number of arguments (!= 2)");
    else if (args[0].isConst() ||
            !args[0].getAccessor()->getInterpretation<Entity::Ptr>())
        throw rete::NodeBuilderException("First argument must be bound to an Entity.");
    else if (args[1].isConst() ||
            !args[1].getAccessor()->getInterpretation<Component::Ptr>())
        throw rete::NodeBuilderException("Second argument must be bound to a Component.");
        /*
        throw rete::NodeBuilderException("Second argument must be bound to a " + std::string(ComponentName<C>::value) + ".");
        */
    // clone the accessors
    rete::PersistentInterpretation<Entity::Ptr>
        entity(args[0].getAccessor()->getInterpretation<Entity::Ptr>()
                                    ->makePersistent());

    rete::PersistentInterpretation<Component::Ptr>
        component(args[1].getAccessor()->getInterpretation<Component::Ptr>()
                                       ->makePersistent());

    // create the node
    auto node = std::make_shared<InferECNode>(std::move(entity), std::move(component));
    return node;
}

}
