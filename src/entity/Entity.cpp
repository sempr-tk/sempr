#include <sempr/entity/Entity.hpp>
#include <sempr/core/EntityEvent.hpp>
#include <sempr/core/EventBroker.hpp>
#include <cassert>
#include <Entity_odb.h>

namespace sempr { namespace entity {

Entity::Entity(const core::IDGenBase* idgen)
    : DBObject(idgen), announced_(false)
{
    // get the eventbroker
    broker_ = core::EventBroker::getInstance();

    // initialize the discriminator to recognize the most derived type of this
    // object to be "Entity". Has to be repeated in every sub-class.
    setDiscriminator<Entity>();
}

// note: constructor "chaining" is only valid since c++11 and is called
// "delegating constructors". One major restriction: You may *NOT*
// initialize any other member variable!
// --> this is invalid:
//  Entity::Entity() : Entity(core::IDGen<Entity>()), foo_(123) { }
// --> but this is okay:
//  Entity::Entity() : Entity(core::IDGen<Entity>()) { foo_ = 123; }
/**
    Default: use IDGen<Entity>
*/
Entity::Entity() : Entity(new core::IDGen<Entity>())
{
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

    // first, announce all children
    for (auto child : children_) {
        child->created();
    }

    baseCalled_ = false;
    created_impl();
    assert(baseCalled_ &&
        "The base method Entity::created_impl() "
        "has not been called during Entity::created()!");
    announced_ = true;
}

void Entity::loaded() {
    if (announced_) return; // do not announce twice.

    // first, announce all children
    for (auto child : children_) {
        child->loaded();
    }

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

    // in the end, announce all children
    for (auto child : children_) {
        child->removed();
    }
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
    /**
        Two vectors to save the child into:
        1. the persistent children_ vector that gets stored in the database
        2. the transient newChildren_ vector

        newChildren_ is traversed during pre[persist/update] to persist the
        children, so that the persistent children_-vector only contains valid
        references.
    */
    children_.push_back(child);
    newChildren_.push_back(child);
}


void Entity::freeChildIDs()
{
    // only need to traverse newChildren_:
    // every DBObject revokes its own ID in preLoad, but the newChildren_
    // are freshly created and not known to odb, hence no preLoad-call.
    for (auto c : newChildren_)
    {
        c->idgenerator_->revoke(c->id());
        c->freeChildIDs();
    }
}

void Entity::prePersist(odb::database &db) const
{
    DBObject::prePersist(db);
    handleChildrenPre(db);
}

void Entity::postPersist(odb::database &db) const
{
    DBObject::postPersist(db);
    handleChildrenPost(db);
}

void Entity::preUpdate(odb::database &db) const
{
    DBObject::preUpdate(db);
    handleChildrenPre(db);
}

void Entity::postUpdate(odb::database &db) const
{
    DBObject::postUpdate(db);
    handleChildrenPost(db);
}

void Entity::preLoad(odb::database& db)
{
    DBObject::preLoad(db);
    /**
        https://git.hb.dfki.de/nniemann/SEMPR/issues/2

        Upon loading an entity, odb creates it with its default ctor, which
        might create and register new children! If the entity is saved again,
        the new children solely exist in the children_ vector, but their
        originally intended member_-variable (e.g. mother_, cadmodel_, ...)
        will be overwritten with the value that was stored previously.

        The fix is the following: Just clear the newChildren_-vector on preLoad.
        Don't be afraid if you look into the registerChildEntity-method: Yes,
        it pushes new children into newChildren_ *AND* children_ simultanously.
        However, we only need to clear newChildren_: These are the ones that get
        handled upon persisting/updating the entity. But aren't the invalid,
        duplicated entities within children_ persisted, too, you ask? No!
        Because that vector gets overwritten by odb when the object is loaded
        and filled with data. :)
        What we also need to do: The newChildren_ got an ID that we want to
        release again!
    */
    freeChildIDs();
    newChildren_.clear();
}

void Entity::handleChildrenPre(odb::database &db) const
{
    for (auto child : newChildren_)
    {
        // set the broker first, so that the child may fire events after it has
        // been persisted. This needs to be done now, because the child may have
        // children itself that it wants to fire events for in the cause of
        // its "persist".
        child->broker_ = broker_;
        //db.persist(child);
    }
}

void Entity::handleChildrenPost(odb::database &db) const
{
    // set the childrens parent to this.
    for (auto child : newChildren_)
    {
        child->setParent(shared_from_this());
        //db.update(child);
    }

    // no need to do this ever again:
    newChildren_.clear();

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
