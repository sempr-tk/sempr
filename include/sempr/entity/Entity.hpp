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
    "Tagged" wraps a shared_ptr to C in a pair together with a string-tag.
    Used to store Tagged<Component>s in an entity, and to return more
    specific Tagged<SpecificComponent>s when asked for it.
*/
template <class C> 
    using Tagged = std::pair<std::shared_ptr<C>, std::string>;


/**
    Collection of data with application-specific semantics.
    Every entity is composed of an id and a list of components that define its
    properties, i.e. the data belonging to it.

TODO: Inherit rete::WME, or create a rete::WME that wraps Entity?
*/
class Entity {
    std::weak_ptr<sempr::core::Core> core_;
    std::vector<Tagged<Component>> components_;
public:
    Entity();
    virtual ~Entity(){}

    using Ptr = std::shared_ptr<Entity>;

    /**
        Adds a given component as a part of this entity.
        An optional string-tag can be provided to differentiate between
        multiple components of the same type later on, if so desired.
    */
    void addComponent(Component::Ptr, const std::string& = "");

    /**
        Removes a known component from the entity.
    */
    void removeComponent(Component::Ptr);

    /**
        Returns a list of all components of the given type, with their tags.

        TODO: This uses dynamic_casts, which could probably be replaced by simple string or integer-checks plus a static_cast. But I'll postpone optimizing stuff for now, that would just add more complexity.
    */
    template <class C>
    std::vector<Tagged<C>>
        getComponents() const
    {
        std::vector<Tagged<C>> results;
        for (auto& component : components_)
        {
            auto ptr = std::dynamic_pointer_cast<C>(component.first);
            if (ptr)
            {
                results.push_back({ptr, component.second});
            }
        }
    }
};

}}

#endif /* end of include guard: SEMPR_ENTITY_ENTITY_H_ */
