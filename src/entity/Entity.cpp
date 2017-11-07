#include <sempr/entity/Entity.hpp>
#include <sempr/core/EntityEvent.hpp>
#include <sempr/core/EventBroker.hpp>
#include <cassert>
#include <Entity_odb.h>

namespace sempr { namespace entity {

Entity::Entity()
{
    // initialize the discriminator to recognize the most derived type of this
    // object to be "Entity". Has to be repeated in every sub-class.
    setDiscriminator<Entity>();
}

void Entity::fireEvent(core::Event::Ptr e) {
    core::EventBroker::Ptr b = broker_.lock();
    if (b) {
        b->enqueueEvent(e);
    }
}

void Entity::changed() {
    baseCalled_ = false;
    changed_impl();
    assert(baseCalled_ &&
        "The base method Entity::changed_impl() "
        "has not been called during Entity::changed()!");
}

void Entity::created() {
    baseCalled_ = false;
    created_impl();
    assert(baseCalled_ &&
        "The base method Entity::created_impl() "
        "has not been called during Entity::created()!");
}

void Entity::loaded() {
    baseCalled_ = false;
    loaded_impl();
    assert(baseCalled_ &&
        "The base method Entity::loaded_impl() "
        "has not been called during Entity::loaded()!");
}

void Entity::removed() {
    baseCalled_ = false;
    removed_impl();
    assert(baseCalled_ &&
        "The base method Entity::removed_impl() "
        "has not been called during Entity::removed()!");
}

void Entity::changed_impl() {
    Event::Ptr e = std::make_shared<Event>(shared_from_this(), Event::CHANGED);
    fireEvent(e);
    baseCalled_ = true;
}

void Entity::created_impl() {
    Event::Ptr e = std::make_shared<Event>(shared_from_this(), Event::CREATED);
    fireEvent(e);
    baseCalled_ = true;
}

void Entity::loaded_impl() {
    Event::Ptr e = std::make_shared<Event>(shared_from_this(), Event::LOADED);
    fireEvent(e);
    baseCalled_ = true;
}

void Entity::removed_impl() {
    Event::Ptr e = std::make_shared<Event>(shared_from_this(), Event::REMOVED);
    fireEvent(e);
    baseCalled_ = true;
}

} /* entity */

} /* sempr */
