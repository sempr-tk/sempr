#include <sempr/entity/Entity.hpp>
#include <sempr/core/EntityEvent.hpp>
#include <sempr/core/EventBroker.hpp>
#include <cassert>
#include <Entity_odb.h>

namespace sempr { namespace entity {

Entity::Entity() : announced_(false)
{
    // get the eventbroker
    broker_ = core::EventBroker::getInstance();

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
    if (!announced_) return;  // only publish changes if it has been announced (created() or loaded())

    baseCalled_ = false;
    changed_impl();
    assert(baseCalled_ &&
        "The base method Entity::changed_impl() "
        "has not been called during Entity::changed()!");
}

void Entity::created() {
    if (announced_) return; // do not announce twice.

    baseCalled_ = false;
    created_impl();
    assert(baseCalled_ &&
        "The base method Entity::created_impl() "
        "has not been called during Entity::created()!");
    announced_ = true;
}

void Entity::loaded() {
    if (announced_) return; // do not announce twice.
    
    baseCalled_ = false;
    loaded_impl();
    assert(baseCalled_ &&
        "The base method Entity::loaded_impl() "
        "has not been called during Entity::loaded()!");
    announced_ = true;
}

void Entity::removed() {
    if (!announced_) return;
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



void Entity::registerChildEntity(Entity::Ptr child)
{
    children_.push_back(child);
}


void Entity::prePersist(odb::database &db) const
{
    handleChildrenPre(db);
}

void Entity::postPersist(odb::database &db) const
{
    handleChildrenPost(db);
}

void Entity::preUpdate(odb::database &db) const
{
    handleChildrenPre(db);
}

void Entity::postUpdate(odb::database &db) const
{
    handleChildrenPost(db);
}


void Entity::handleChildrenPre(odb::database &db) const
{
    std::cout << discriminator() << "  PRE" << '\n';
    for (auto child : children_)
    {
        // set the broker first, so that the child may fire events after it has
        // been persisted. This needs to be done now, because the child may have
        // children itself that it wants to fire events for in the cause of
        // its "persist".
        child->broker_ = broker_;
        db.persist(child);
    }
}

void Entity::handleChildrenPost(odb::database &db) const
{
    std::cout << discriminator() << "  POST" << '\n';

    // set the childrens parent to this.
    for (auto child : children_)
    {
        child->setParent(shared_from_this());
        db.update(child);
    }
    // persist first, process second
    for (auto child : children_)
    {
        child->created();
    }

    // no need to do this ever again:
    children_.clear();

    /*
        Cite from the documentation (p. 223):
        > If you need to modify the object in one of the "const" events, then
        > you can safely cast away const-ness using the const_cast operator if
        > you know that none of the objects will be created const.

        Could be useful someday. Right now, I just made children_ mutable and
        the parent_-pointer point to a const DBObject. We won't need to modify
        the parent through that pointer, I guess, it's just there for the
        on-delete-cascade. But, if we eventually do, just make it non-const again
        and cast away the const-ness in child->setParent using
        std::const_pointer_cast<DBObject>(shared_from_this()).
    */
}


} /* entity */

} /* sempr */
