#ifndef SEMPR_CORE_ENTITYALPHABUILDER_HPP_
#define SEMPR_CORE_ENTITYALPHABUILDER_HPP_

#include "EntityAlphaNode.hpp"
#include "EntityAccessor.hpp"
#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

namespace sempr {
namespace core {

/**
    Templated class to build EntityAlphaNodes of different types.
*/
template <class EntityType>
class EntityAlphaBuilder : public rete::NodeBuilder {
public:
    /**
        Constructs a new NodeBuilder that creates EntityAlphaNodes from conditions with the given
        name.
            EntityAlphaBuilder<sempr::entity::Polygon>("Polygon");
        will allow you to write
            [Polygon(?p, ?id), ... -> ...]
        in your rules.
    */
    EntityAlphaBuilder(const std::string& name)
        : rete::NodeBuilder(name, BuilderType::ALPHA)
    {
    }

    /**
        Creates new Nodes (actually just one) from an argument list and places them in the nodes
        vector. The ArgumentList must have exactly two entries, both unbound variables, which will
        be bound to 1. the entity itself and 2. the id of the entity.
        The id of the entity will be in the URI compatible form, so instead of "Entity_7" it will
        read "<sempr:Entity_7>".
    */
    void buildAlpha(rete::ArgumentList& args, std::vector<rete::AlphaNode::Ptr>& nodes) const override
    {
        if (args.size() != 2) throw rete::NodeBuilderException("Wrong number of arguments (!= 2)");
        if (!args[0].isVariable() || args[0].getAccessor() != nullptr)
            throw rete::NodeBuilderException("Arguments must be unbound variables");
        if (!args[1].isVariable() || args[1].getAccessor() != nullptr)
            throw rete::NodeBuilderException("Arguments must be unbound variables");

        typename EntityAlphaNode<EntityType>::Ptr node(new EntityAlphaNode<EntityType>());
        nodes.push_back(node);

        // create the entity accessors and bind the variables!
        auto entityAccessor = std::make_shared<EntityAccessor<EntityType>>();
        auto idAccessor = std::make_shared<EntityIDAccessor>();

        args[0].bind(entityAccessor);
        args[1].bind(idAccessor);
    }
};

} /* core */
} /* sempr */

#endif /* end of include guard: SEMPR_CORE_ENTITYALPHABUILDER_HPP__*/
