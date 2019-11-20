#ifndef SEMPR_CORE_ENTITYALPHANODE_HPP_
#define SEMPR_CORE_ENTITYALPHANODE_HPP_

#include <rete-core/ReteCore.hpp>
#include "sempr/core/EntityWME.hpp"
#include "sempr/core/Utility.hpp"
namespace sempr {
namespace core {

/**
    Implementation of a rete::AlphaNode which checks for the entity type.
    It is templated to allow registering multiple specializations, for the different types one wants
    to check for.
    I currently can't think of a nice way to do the
        "check any entity for any type specified at runtime / in the rules".
    Hence this is a compromise:
        - For every type of entity to check for you have a different type of alpha node
        - The specific instantiation does a dynamic_cast to figure out if the type is okay.
        - Everything is templated, such that it is as easy to extend as the math builtins are added,
                registerNodeBuilder<EntityAlphaBuilder<entity::Geometry>>();
            Where EntityAlphaBuilder instantiates EntityAlphaNode<entity::Geometry>,
            which knows how to do the check using dynamic_cast.
    NOTE: We could also just do a simple check on the discriminator, but we would only get leaf-types with that. "Entity(?e, 'sempr::entity::RDFEntity')" would not match RDFVector, etc.
*/
template <class EntityType>
class EntityAlphaNode : public rete::AlphaNode {
    std::string name_;

    std::string getDOTAttr() const override
    {
        return "[label=\"" + rete::util::dotEscape(name_) + "\"]";
    }
public:
    using Ptr = std::shared_ptr<EntityAlphaNode>;

    EntityAlphaNode(const std::string& name)
        : name_(name)
    {
        static_assert(std::is_base_of<sempr::entity::Entity, EntityType>::value, "Tried to add a entity-type-check for a non-entity node!");
    }

    EntityAlphaNode()
        : EntityAlphaNode(odb_discriminator<EntityType>::value)
    {
    }

    /**
        Checks if a given WME fulfills the condition to check (is of correct type),
        and propagates its addition/removal
    */
    void activate(rete::WME::Ptr wme, rete::PropagationFlag flag) override
    {
        if (flag == rete::PropagationFlag::RETRACT)
        {
            // shortcut: why check anything? just propagate!
            propagate(wme, rete::PropagationFlag::RETRACT);
            return;
        }

        // check one: is it even a EntityWME?
        if (EntityWME::Ptr ewme = std::dynamic_pointer_cast<EntityWME>(wme))
        {
            // check two: is the contained Entity of the type we want?
            if (dynamic_cast<EntityType*>(ewme->entity().get()))
            {
                propagate(wme, flag);
            }
        }
    }

    /**
        Equality with another AlphaNode is given if the other one has the same type.
        We only need one EntityAlphaNode<sempr::entity::Polygon> to check for Polygon entities...
    */
    bool operator == (const AlphaNode& other) const override
    {
        // the condition is fully specified in its c++ type.
        // dynamic_cast in one direction is enough as there is no deeper inheritance hierarchy,
        // no other node inherits this.
        // Else we could compare typeids here.
        if (const EntityAlphaNode* o = dynamic_cast<const EntityAlphaNode*>(&other))
        {
            return true;
        }
        return false;
    }
};

} /* core */
} /* sempr */


#endif /* end of include guard: SEMPR_CORE_ENTITYALPHANODE_HPP_ */
