#ifndef SEMPR_INFERECBUILDER_HPP_
#define SEMPR_INFERECBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

#include "InferECNode.hpp"
#include "../Utility.hpp"

namespace sempr {

/**
    Constructs the InferECNodes, which are used to make computed components
    visible to the reasoner by associating them with entities.
    (Note: The entity will *not* be visible to the component object! This is
           only for the reasoner!)
    [... -> EC(?entity ?component)]

    Setting tags is not supported, as this would enable overwriting tags (which
    are stored at the component, not in the ECWME).

    NOTE: This is templated to allow different component types. This is really
    strange, since the ECWME does not care, all it needs is an Component::Ptr.
    The problem is the Accessor-System, which is not as nice and flexible as I
    thought: E.g., a rete::SpecificTypeAccessor<AffineTransform::Ptr> cannot be
    cast to a        rete::SpecificTypeAccessor<Component::Ptr>, as the
    hierarchy of accessors does not follow the hierarchy of the values they are
    able to access. And it would not be a good idea to try to enforce exactly
    that, as it would only lead to structures as in the old version of sempr...
    with the entities and EntityEvents...:(
*/
template <class C>
class InferECBuilder : public rete::NodeBuilder {
public:
    InferECBuilder()
        : rete::NodeBuilder("EC<" + std::string(ComponentName<C>::value) + ">",
                            rete::NodeBuilder::BuilderType::EFFECT)
    {
    }

    rete::Production::Ptr buildEffect(rete::ArgumentList& args) const override
    {
        // needs exactly 2 args, which must be vars bound to an entity and a
        // component
        if (args.size() != 2)
            throw rete::NodeBuilderException("Wrong number of arguments (!= 2)");
        else if (args[0].isConst() ||
                !args[0].getAccessor()->getInterpretation<Entity::Ptr>())
            throw rete::NodeBuilderException("First argument must be bound to an Entity.");
        else if (args[1].isConst() ||
                !args[1].getAccessor()->getInterpretation<std::shared_ptr<C>>())
            throw rete::NodeBuilderException("Second argument must be bound to a " + std::string(ComponentName<C>::value) + ".");
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
};


}

#endif /* include guard: SEMPR_INFERECBUILDER_HPP_ */

