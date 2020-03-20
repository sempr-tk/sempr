#ifndef SEMPR_INFERECBUILDER_HPP_
#define SEMPR_INFERECBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

#include "nodes/InferECNode.hpp"
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

    But the Accessors need some overhaul/rethinking. Maybe we should simplify
    them, and remove the inheritance/casting stuff from them, too. Instead we
    could have a single (!) Acceptor class, or maybe one templated for the very
    concrete type it points at, and give it capabilities to convert the pointed
    at value to a requested one. E.g. in a map[typeid] -> function, where the
    typeid specifies the *return* type. And a templated getter could lookup the
    capabilities in the map, call the method and return the value. Easy. Right?

    I'm going to add this as an issue to rete project...
    -> https://git.ni.dfki.de/sempr/rete/issues/15

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
        if (args.size() != 2) throw rete::NodeBuilderException("Wrong number of arguments (!= 2)");
        if (args[0].isConst() || !args[0].getAccessor()->canAs<rete::SpecificTypeAccessor<Entity::Ptr>>())
            throw rete::NodeBuilderException("First argument must be bound to an Entity.");
        if (args[1].isConst() || !args[1].getAccessor()->canAs<rete::SpecificTypeAccessor<std::shared_ptr<C>>>())
            throw rete::NodeBuilderException("Second argument must be bound to a " + std::string(ComponentName<C>::value) + ".");
        // clone the accessors
        std::unique_ptr<rete::SpecificTypeAccessor<Entity::Ptr>>
            entity(args[0].getAccessor()->clone()->as<rete::SpecificTypeAccessor<Entity::Ptr>>());

        std::unique_ptr<rete::SpecificTypeAccessor<Component::Ptr>>
            component(args[1].getAccessor()->clone()->as<rete::SpecificTypeAccessor<Component::Ptr>>());

        // create the node
        auto node = std::make_shared<InferECNode>(std::move(entity), std::move(component));
        return node;
    }
};


}

#endif /* include guard: SEMPR_INFERECBUILDER_HPP_ */

