#ifndef SEMPR_ENTITY_ENTITY_H_
#define SEMPR_ENTITY_ENTITY_H_

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <map>

// enable json serialization for all entities
#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/access.hpp>

#include "Component.hpp"
#include "Exception.hpp"


namespace sempr {
    class Core;

/**
    Collection of data with application-specific semantics.
    Every entity is composed of an id and a list of components that define its
    properties, i.e. the data belonging to it.

TODO: Inherit rete::WME, or create a rete::WME that wraps Entity?
*/
class Entity : public std::enable_shared_from_this<Entity> {
    template <class T = Component>
    using ComponentTag = std::pair<std::shared_ptr<T>, std::string>;

private:
    /// raw pointer, set and unset by the core when the entity is added/removed.
    Core* core_;
    std::string id_;
    bool idIsURI_;
    bool isTemporary_; // temporary entities are not persisted!

    std::map<Component::Ptr, std::string> components_; // with tag

    friend class Core;
    friend class cereal::access;

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
    static Entity::Ptr createTemporary();

    /**
        Returns a pointer to the currently set core of the entity.
TODO: I'm not sure if this should be exposed here. I added it to allow the Component-class (which has a pointer to entity) to notify the core about an internal update. I could also make the component a friend of entity, but in that case every component can see all components added to the entity, which would be wrong, as it could lead to cross-dependencies.
    */
    Core* core() const;


    /**
     * Returns if the entity was created to be temporary and thus
     * must not be persisted
    */
    bool isTemporary() const;

    /**
        Get the entities id.
        Returns an empty string if the id is not yet set (i.e., the entity was
        not added to the core, yet).
    */
    std::string id() const;

    /**
        Returns true if the id has been set by setURI(...)
    */
    bool idIsURI() const;

    /**
        Sets the entities id. Useful if you want to explicitely identify
        specific entities and thus assign hard-coded ids.
        If you don't set an ID, the persistence layer in the core will.

        \throws an exception if the entity already has an id.
    */
    void setId(const std::string&);

    /**
        Set the entities id, but flags it as an URI.
        This prevents padding it in "<sempr:" + id + ">" when interpreting
        the entity as a TriplePart.
    */
    void setURI(const std::string&);

    /**
        Adds a given component as a part of this entity.
        Sets the tag to an empty string.
    */
    void addComponent(Component::Ptr);

    /**
        Adds a given component as a part of this entity, assigned with a
        specific tag.
    */
    void addComponent(Component::Ptr, const std::string& tag);

    /**
        Removes a known component from the entity.
        (Tag does not matter, as each component can only be once assigned to
        the entity, thus only have one tag).
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
        for (auto ct : components_)
        {
            auto component = ct.first;
            auto ptr = std::dynamic_pointer_cast<C>(component);
            if (ptr)
            {
                results.push_back(ptr);
            }
        }
        return results;
    }

    template <class C>
    std::vector<ComponentTag<C>> getComponentsWithTag() const
    {
        std::vector<ComponentTag<C>> results;
        for (auto ct : components_)
        {
            auto component = ct.first;
            auto ptr = std::dynamic_pointer_cast<C>(component);
            if (ptr)
            {
                results.push_back(ComponentTag<C>(ptr, ct.second));
            }
        }
        return results;
    }

    std::string getTag(Component::Ptr c) const
    {
        auto it = components_.find(c);
        if (it != components_.end())
        {
            return it->second;
        }
        else
        {
            throw sempr::Exception("Component is not part of this entity.");
        }
    }


    /**
        Serialization with cereal
    */
    template <class Archive>
    void save(Archive& ar) const
    {
        ar(
            cereal::make_nvp<Archive>("id", id_),
            cereal::make_nvp<Archive>("idIsURI", idIsURI_),
            cereal::make_nvp<Archive>("components", components_)
        );
    }

    template <class Archive>
    void load(Archive& ar)
    {
        ar(
            cereal::make_nvp<Archive>("id", id_),
            cereal::make_nvp<Archive>("idIsURI", idIsURI_),
            cereal::make_nvp<Archive>("components", components_)
        );

        // have to reset the components entity-ptr!
        for (auto c : components_)
        {
            c.first->entity_ = this;
        }
    }
};

}


CEREAL_REGISTER_TYPE(sempr::Entity)

// Also: Add specialization for rete::util::to_string to be able to use it in
// the rete::TupleWME

#include <rete-core/Util.hpp>

namespace rete { namespace util {

template <> std::string to_string(const sempr::Entity& e);
template <> std::string to_string(const sempr::Entity::Ptr& e);

}}

#endif /* end of include guard: SEMPR_ENTITY_ENTITY_H_ */
