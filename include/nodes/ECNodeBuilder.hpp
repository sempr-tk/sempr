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
        Creates the ECNode<C>. Needs two or three entries in the argument list,
        the first two being unbound variables which will be bound to the
        Entity::Ptr and the C(omponent)::Ptr. The third (optional) value refers
        to the string-tag of the component and can either be an unbound
        variable, in which case it will be bound to the tag value, or a bound
        variable or a constant value, in which case the constructed alpha node
        will check for the tag.
    */
    void buildAlpha(rete::ArgumentList& args, std::vector<rete::AlphaNode::Ptr>& nodes) const override
    {
        if (args.size() < 2) throw rete::NodeBuilderException("Too few arguments, need at least 2 (entity and component)");
        if (args.size() > 3) throw rete::NodeBuilderException("Too many arguments, max 3 supported (entity, component, tag)");

        if (!args[0].isVariable() || args[0].getAccessor() != nullptr)
            throw rete::NodeBuilderException("Argument 0 must be unbound variable");
        if (!args[1].isVariable() || args[1].getAccessor() != nullptr)
            throw rete::NodeBuilderException("Argument 1 must be unbound variable");
        if (args.size() == 3 && args[2].isVariable() && args[2].getAccessor() != nullptr)
            throw rete::NodeBuilderException("Argument 2 must be const or unbound variable");


        // construct the node
        typename ECNode<C>::Ptr node(
                    new ECNode<C>(ComponentName<C>::value)
                );
        nodes.push_back(node);

        // need to deal with the tag-argument?
        if (args.size() == 3)
        {
            if (args[2].isConst())
            {
                // add a node to check the tag
                auto tagcheck = std::make_shared<ComponentTagNode>(args[2].getAST());
                nodes.push_back(tagcheck);
            }
            else
            {
                // unbound variable. Bind it.
                auto tagAccessor = std::make_shared<ComponentTagAccessor>();
                args[2].bind(tagAccessor);
            }
        }


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
