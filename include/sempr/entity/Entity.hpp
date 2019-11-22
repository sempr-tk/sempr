#ifndef SEMPR_ENTITY_ENTITY_H_
#define SEMPR_ENTITY_ENTITY_H_

#include <string>
#include <vector>
#include <utility>
#include <memory>

#include "Component.hpp"


namespace sempr {
    namespace core {
        class Core;
    }

namespace entity {

/**
    Collection of data with application-specific semantics.
    Every entity is composed of an id and a list of components that define its
    properties, i.e. the data belonging to it.

TODO: Inherit rete::WME, or create a rete::WME that wraps Entity?
*/
class Entity : public std::enable_shared_from_this<Entity> {
    /// raw pointer, set and unset by the core when the entity is added/removed.
    core::Core* core_;
    std::string id_;
    std::vector<Component::Ptr> components_;

    friend class sempr::core::Core;

protected:
    /// protected ctor because entities must never be created on the stack
    // (because they inherit std::enable_shared_from_this)
    Entity();
public:
    virtual ~Entity();

    using Ptr = std::shared_ptr<Entity>;

    /**
        Creates a new Entity.
    */
    static Entity::Ptr create();

    /**
        Returns a pointer to the currently set core of the entity.
TODO: I'm not sure if this should be exposed here. I added it to allow the Component-class (which has a pointer to entity) to notify the core about an internal update. I could also make the component a friend of entity, but in that case every component can see all components added to the entity, which would be wrong, as it could lead to cross-dependencies.
    */
    sempr::core::Core* core() const;

    /**
        Get the entities id.
        Returns an empty string if the id is not yet set (i.e., the entity was
        not added to the core, yet).
    */
    std::string id() const;

    /**
        Sets the entities id. Useful if you want to explicitely identify
        specific entities and thus assign hard-coded ids.
        If you don't set an ID, the persistence layer in the core will.

        \throws an exception if the entity already has an id.
    */
    void setId(const std::string&);

    /**
        Adds a given component as a part of this entity.
    */
    void addComponent(Component::Ptr);

    /**
        Removes a known component from the entity.
    */
    void removeComponent(Component::Ptr);

    /**
        Returns a list of all components of the given type.

        TODO: This uses dynamic_casts, which could probably be replaced by simple string or integer-checks plus a static_cast. But I'll postpone optimizing stuff for now, that would just add more complexity.
    */
    template <class C>
    std::vector<std::shared_ptr<C>>
        getComponents() const
    {
        std::vector<std::shared_ptr<C>> results;
        for (auto component : components_)
        {
            auto ptr = std::dynamic_pointer_cast<C>(component);
            if (ptr)
            {
                results.push_back(ptr);
            }
        }
        return results;
    }
};

}}


// Also: Add specialization for rete::util::to_string to be able to use it in
// the rete::TupleWME

#include <rete-core/Util.hpp>

namespace rete { namespace util {

template <> std::string to_string(const sempr::entity::Entity& e);
template <> std::string to_string(const sempr::entity::Entity::Ptr& e);

}}

#endif /* end of include guard: SEMPR_ENTITY_ENTITY_H_ */
