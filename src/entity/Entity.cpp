#include <sempr/entity/Entity.hpp>
#include <sempr/core/EntityEvent.hpp>
#include <sempr/core/EventBroker.hpp>
#include <cassert>
#include <Entity_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(Entity)

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
    } else {
        std::cerr << "Warning: Entity tried to fire an event, but no broker is set." << '\n';
    }
}

void Entity::changed() {
    if (!announced_) return;  // only publish changes if it has been announced (created() or loaded())

    auto e = this->createEntityEvent(core::EntityEventBase::CHANGED);
    fireEvent(e);
}

void Entity::created() {
    if (announced_) return; // do not announce twice.

    // first, announce all children
    for (auto child : children_) {
        child->created();
    }

    auto e = this->createEntityEvent(core::EntityEventBase::CREATED);
    fireEvent(e);
    announced_ = true;
}


void Entity::loaded() {
    if (announced_) return; // do not announce twice.

    // first, announce all children
    for (auto child : children_) {
        child->loaded();
    }

    auto e = this->createEntityEvent(core::EntityEventBase::LOADED);
    fireEvent(e);
    announced_ = true;
}

void Entity::removed() {
    if (!announced_) return;

    auto e = this->createEntityEvent(core::EntityEventBase::REMOVED);
    fireEvent(e);

    // in the end, announce all children
    for (auto child : children_) {
        child->removed();
    }
}


void Entity::registerChildEntity(Entity::Ptr child)
{
    /**
        Two vectors to save the child into:
        1. the persistent children_ vector that gets stored in the database
        2. the transient newChildren_ vector

        newChildren_ is only used to handle the case in which chilren get created and registered
        only to be overwritten during load. The children_-vector is the actual set of children
        that have also been saved in the database, or will be saved, possible, later on.
        Entity::created / loaded / removed will call the respecting method of the children_, too.
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
}

void Entity::postPersist(odb::database &db) const
{
    DBObject::postPersist(db);
    newChildren_.clear();
}

void Entity::preUpdate(odb::database &db) const
{
    DBObject::preUpdate(db);
}

void Entity::postUpdate(odb::database &db) const
{
    DBObject::postUpdate(db);
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
        it pushes new children into newChildren_ *AND* children_ simultanously, but children_ is
        persistent and will be overwritten during load.

        What we also need to do: The newChildren_ got an ID that we want to
        release again!
    */
    freeChildIDs(); // TODO: Question: What about freeing the id in the dtor if not persistent? What about freeing the id of a removed entity?
    newChildren_.clear();
}


} /* entity */

} /* sempr */
