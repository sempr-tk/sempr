#include "sempr/core/Exception.hpp"
#include "sempr/entity/Entity.hpp"
#include "sempr/core/Core.hpp"

#include <cassert>
#include <algorithm>
//#include <Entity_odb.h>

namespace sempr { namespace entity {

Entity::Entity() : core_(nullptr), id_("")
{
}

Entity::~Entity()
{
}

Entity::Ptr Entity::create()
{
    return Entity::Ptr(new Entity());
}

core::Core* Entity::core() const
{
    return core_;
}

std::string Entity::id() const
{
    return id_;
}

void Entity::setId(const std::string& id)
{
    if (!id_.empty()) throw sempr::Exception("Entity already has an id");
    id_ = id;
}

void Entity::addComponent(Component::Ptr c)
{
    if (!c) throw sempr::Exception("cannot add nullptr as component");
    if (c->entity_) throw sempr::Exception("Component already part of an entity");

    c->entity_ = this;
    components_.push_back(c);

    // if the entity is already part of a core we need to inform the reasoner
    // about the new component.
    if (core_)
    {
        core_->addedComponent(shared_from_this(), c);
    }
}

void Entity::removeComponent(Component::Ptr c)
{
    if (!c) return;
    if (c->entity_ != this) throw sempr::Exception("Component to remove is not part of this entity");

    // find the component in the entities component-list
    auto it = std::find(components_.begin(), components_.end(), c);

    // should never happen
    if (it == components_.end()) throw sempr::Exception("Component to remove not part of this entity -- but has entity_ set to this");


    // if the entity is already part of a core we need to inform the reasoner
    // about the removed component.
    if (core_)
    {
        core_->removedComponent(shared_from_this(), *it);
    }
    
    (*it)->entity_ = nullptr;
    components_.erase(it);
}


} /* entity */
} /* sempr */


#include <rete-core/Util.hpp>

namespace rete { namespace util {

template <> 
std::string to_string(const sempr::entity::Entity& e)
{
    return e.id();
}

template <>
std::string to_string(const sempr::entity::Entity::Ptr& e)
{
    if (!e) return "nullptr";
    return to_string(*e);
}

}}

