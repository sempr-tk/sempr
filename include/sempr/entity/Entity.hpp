#ifndef SEMPR_ENTITY_ENTITY_H_
#define SEMPR_ENTITY_ENTITY_H_

#include <odb/core.hxx>

#include <wchar.h>
#include <string>
#include <vector>
#include <memory>

#include <sempr/storage/DBObject.hpp>
#include <sempr/core/Event.hpp>
#include <sempr/core/EntityEvent.hpp>


/**
    The SEMPR_ENTITY macro declares a 'createEntityEvent' method. It is first introduced in Entity
    and is used there in the changed(), created(), ... methods to throw an EntityEvent. By
    overriding this method in subclasses we adjust the thrown event to the derived classes. So
    when you call Polygon::changed() you will get a EntityEvent<Polygon> instead of
    EntityEvent<Geometry> or EntityEvent<Entity>. Note: Modules listening for EntityEvent<Geometry>
    will still be able to process an EntityEvent<Polygon>, since the EntityEvents follow the
    inheritance chain of the entity-classes and register their types and the modules internally
    use "bool Observable::isA<class T>()" and "T::Ptr as<class T>(Observable::Ptr)".

    Insert this somewhere in your class declaration, e.g.:
        // Foo.hpp
        #pragma db object
        class Foo : public Entity {
            SEMPR_ENTITY
        public:
            // [...]
        };
*/
#define SEMPR_ENTITY \
    virtual sempr::core::EntityEventBase::Ptr createEntityEvent(sempr::core::EntityEventBase::EventType type) override;

/**
    The SEMPR_ENTITY_SOURCE(CLASS) macro implements the 'createEntityEvent' method declared by
    SEMPR_ENTITY. This has to be used in the source file since it instantiates a template of
    EntityEvent<CLASS>, which internally uses base_of<T> and therefore odb::object_traits<T>.
    But since the odb::object_traits<T> specialization for this class is created by processing the
    header file with the odb-compiler we can only access it in the source file.

    Insert this somewhere in your class definition, source file, e.g.
        // Foo.cpp
        SEMPR_ENTITY_SOURCE(Foo)
            [...]
        Foo::Foo()
        {
            [...]
        }
*/
#define SEMPR_ENTITY_SOURCE(CLASS) \
    sempr::core::EntityEventBase::Ptr CLASS::createEntityEvent(sempr::core::EntityEventBase::EventType type) \
    { \
        return sempr::core::EntityEventBase::Ptr( \
            new sempr::core::EntityEvent<CLASS>(shared_from_base<CLASS>(), type) \
        ); \
    }



namespace sempr {
    namespace core {
        class Core;
        class EventBroker;
        // class Event;
    }

namespace entity {

/**
    Collection of data with application-specific semantics.
*/
#pragma db object
class Entity : public storage::DBObject, public std::enable_shared_from_this<Entity>  {
public:
    Entity();
    Entity(const core::IDGenBase* idgen);

    virtual ~Entity(){}

    using Ptr = std::shared_ptr<Entity>;
    // using Event = core::EntityEvent<Entity>;

    /** Fires an event signalling that this entity changed (EntityEvent<Entity>).
        Derived classes need to override changed_impl() to also fire special
        events, like EntityEvent<RDFEntity>, EntityEvent<GeometricEntity>, ...
    */
    void changed();

    /** TODO: Everyone might modify an entity and thus call entity->changed()
        to trigger the processing pipeline. However, what about created, loaded
        and removed? One could argue that the storage module actually should be
        just a processing-module, too! Then it would react to created() and
        removed(), and one could implement an event or query that leads to the
        storage loading a subset of the entities in the database, triggering
        loaded(). Also, this would allow entities to create other entites on their
        own, as they could just call child->created() and child->removed() to
        persist or delete them, besides using child->changed().

        Well, I drifted away from what I originally wanted to say.
        Should we make created, loaded and removed private? In that case, they
        could still be overridden by derived classes, but could only be used by
        the Core (assuming a "friend class Core" in Entity) --> enforced tasks.

        NOTE: Quite a lot has happened since I wrote the above todo. The Storage has become more or less a processing module: The DBUpdateModule takes care of updating the database, and even answers 'LoadingQuery's. Also, entities create their own child-entities and announce them with child->created() etc... But: I want to rework the Entity soon, and change the way IDs are given, things are initialized, the EventBroker is set, ... so I'll leave this here as a reminder. A nice interface to allow entities and modules to create entities has yet to be worked out.
    */
    void created();
    void loaded();
    void removed();

    /**
        Registers a child-entity: Every entity registered as a child-entity of this will be
        announced (child->created() / child->loaded()) before and removed (child->removed()) after
        this. This allows a class to be made of other entities, e.g. an RDFPropertyMap that
        describes this but is invalid without this.

        Be careful when using this. There are two ways that *should* work fine:

        1. Register the child entity when both parent and child are not yet announced to the system. When you announce the parent it will automatically announce the children first. (Announce = created() or loaded()).
        2. Register the child entity when both parent and child have been announced before.

        I think the main concern is to make sure that the children are persisted before the parent
        in order to not run into any database issues. There are other parts of SEMPR which should
        be reworked sooner or later, which will change this problem and hopefully simplify it -- or
        at least provide a strict, unified, easy way to handle it safely.
    */
    void registerChildEntity(Entity::Ptr child);


protected:
    /** fires an event **/
    void fireEvent(core::Event::Ptr e);

    template<class Derived>
    std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    /** handle registered children */
    virtual void prePersist(odb::database& db) const override;
    virtual void postPersist(odb::database& db) const override;
    virtual void preUpdate(odb::database& db) const override;
    virtual void postUpdate(odb::database& db) const override;

    /**
        On pre-load, discard the newChildren_ that may have been created and
        registered in the constructor, but whose member-variables would be
        overwritten during the load anyways, leaving behind a child entity that
        is solely referred to by the (new)children_-vector.
    */
    virtual void preLoad(odb::database& db) override;

private:
    friend class odb::access;
    friend class core::Core;

    /** used to fire events, set by the core */
    #pragma db transient
    std::weak_ptr<core::EventBroker> broker_;


    /**
        Creates an EntityEvent.
        This has to be overridden by every subclass to return the correct EntityEvent-type
        (e.g. EntityEvent<MySpecialEntity>). Make sure to implement it in the source only, since
        "*_odb.h" *must* be included before using the EntityEvent-class. (It uses the odb object
        traits).
    */
    virtual core::EntityEventBase::Ptr createEntityEvent(core::EntityEventBase::EventType type);
    // SEMPR_ENTITY // same, but with "override"


    /**
        The vector of newChildren_ is only used to free the ids of the children that might have been
        created in the default ctor during pre-load.
    */
    #pragma db transient
    mutable std::vector<Entity::Ptr> newChildren_;


    /**
        Releases the ids of newChildren_. This is needed since the children
        might be created in the ctor of this entity, even when they are
        overwritten in a subsequent load.
    */
    void freeChildIDs();

    /** Besides the list of newly registered children, we need to keep track
        of all our children. When the parent is created, loaded or removed
        we must fire the according events for the children, too. This is done
        in created() / loaded() / removed().
    */
    std::vector<Entity::Ptr> children_;

    /**
        Helper to check if the base implementations of ..._impl() have been
        called.
    */
    #pragma db transient
    bool baseCalled_;

    /**
        Helper to check if this entity has been announced to the system via
        "created()" or "loaded()". If not, "changed()" will _not_ fire an event.
        This behaviour allows us to initialize entities without changed-events
        for every step in between.
    */
    #pragma db transient
    bool announced_;
};

}}

#endif /* end of include guard: SEEPRO_ENTITY_ENTITY_H_ */
