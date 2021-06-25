#include "Exception.hpp"
#include "Entity.hpp"
#include "Core.hpp"

#include <cassert>
#include <algorithm>
//#include <Entity_odb.h>

namespace sempr {

Entity::Entity() : core_(nullptr), id_(""), idIsURI_(false), isTemporary_(false)
{
}

Entity::~Entity()
{
}

Entity::Ptr Entity::create()
{
    return Entity::Ptr(new Entity());
}

Entity::Ptr Entity::createTemporary()
{
    Entity::Ptr e(new Entity());
    e->isTemporary_ = true;
    return e;
}


Core* Entity::core() const
{
    return core_;
}

bool Entity::isTemporary() const
{
    return isTemporary_;
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


void Entity::setURI(const std::string& uri)
{
    setId(uri);
    idIsURI_ = true;
}

bool Entity::idIsURI() const
{
    return idIsURI_;
}


void Entity::addComponent(Component::Ptr c)
{
    addComponent(c, "");
}

void Entity::addComponent(Component::Ptr c, const std::string& tag)
{
    if (!c) throw sempr::Exception("cannot add nullptr as component");
    if (c->entity_) throw sempr::Exception("Component already part of an entity");

    c->entity_ = this;
    components_[c] = tag;

    // if the entity is already part of a core we need to inform the reasoner
    // about the new component.
    if (core_)
    {
        core_->addedComponent(shared_from_this(), c, tag);
    }
}

void Entity::removeComponent(Component::Ptr c)
{
    if (!c) return;
    if (c->entity_ != this) throw sempr::Exception("Component to remove is not part of this entity");

    // find the component in the entities component-list
    auto it = components_.find(c);

    // should never happen
    if (it == components_.end()) throw sempr::Exception("Component to remove not part of this entity -- but has entity_ set to this");


    // if the entity is already part of a core we need to inform the reasoner
    // about the removed component.
    if (core_)
    {
        core_->removedComponent(shared_from_this(), it->first, it->second);
    }

    it->first->entity_ = nullptr;
    components_.erase(it);
}

} /* sempr */


#include <rete-core/Util.hpp>

namespace rete { namespace util {

template <>
std::string to_string(const sempr::Entity& e)
{
    return e.id();
}

template <>
std::string to_string(const sempr::Entity::Ptr& e)
{
    if (!e) return "nullptr";
    return to_string(*e);
}

}}

