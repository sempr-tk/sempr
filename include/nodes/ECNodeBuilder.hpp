#ifndef SEMPR_ECNODEBUILDER_HPP_
#define SEMPR_ECNODEBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

#include "ECNode.hpp"
#include "ECAccessors.hpp"
#include "Utility.hpp"

namespace sempr {

/**
    AlphaNode-Builders for any component type
    (--> EC<Component>(?entity ?component) condition)
*/
template <class C>
class ECNodeBuilder : public rete::NodeBuilder {
public:
    /**
        Creates a new node builder for the component type given in C.
        It will be used to create ECNode<C> instances from conditions of the
        form
            EC<Component>(?entity ?component)
        where "Component" is the name given to the component type, as defined
        in
            ComponentName<C>::value
    */
    ECNodeBuilder()
        : rete::NodeBuilder(
                // note: ComponentName<C>::value is a const char*!
                "EC<" + std::string(ComponentName<C>::value) + ">",
                rete::NodeBuilder::BuilderType::ALPHA)
    {
    }

    /**
        Creates the ECNode<C>. Needs exactly two entries in the argument list,
        both unbound variables (there is no direct check for an ID or similar).

        The first argument will be bound to the Entity::Ptr, the second to the
        C(omponent)::Ptr.
    */
    void buildAlpha(rete::ArgumentList& args, std::vector<rete::AlphaNode::Ptr>& nodes) const override
    {
        if (args.size() != 2) throw rete::NodeBuilderException("Wrong number of arguments (!= 2)");
        if (!args[0].isVariable() || args[0].getAccessor() != nullptr)
            throw rete::NodeBuilderException("Arguments must be unbound variables");
        if (!args[1].isVariable() || args[1].getAccessor() != nullptr)
            throw rete::NodeBuilderException("Arguments must be unbound variables");

        typename ECNode<C>::Ptr node(
                    new ECNode<C>(ComponentName<C>::value)
                );
        nodes.push_back(node);

        // create the accessors
        auto entityAccessor = std::make_shared<EntityAccessor>();
        auto componentAccessor = std::make_shared<ComponentAccessor<C>>();

        // and bind the variables
        args[0].bind(entityAccessor);
        args[1].bind(componentAccessor);
    }

};

}

#endif /* include guard SEMPR_ECNODEBUILDER_HPP_ */
