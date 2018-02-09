#ifndef SEMPR_ENTITY_ENTITY_H_
#define SEMPR_ENTITY_ENTITY_H_

#include <odb/core.hxx>

#include <string>
#include <vector>
#include <memory>

#include <sempr/storage/DBObject.hpp>
#include <sempr/core/Event.hpp>
#include <sempr/core/EntityEvent.hpp>


#define ENTITY_DEFAULT_CHANGED(CLASS, BASE) \
    virtual void changed_impl() override { \
        BASE::changed_impl(); \
        auto e = std::make_shared<sempr::core::EntityEvent<CLASS> >(  \
            shared_from_base<CLASS>(), sempr::core::EntityEventBase::CHANGED  \
        );  \
        fireEvent(e);  \
    }

#define ENTITY_DEFAULT_CREATED(CLASS, BASE) \
    virtual void created_impl() override { \
        BASE::created_impl(); \
        auto e = std::make_shared<sempr::core::EntityEvent<CLASS> >(  \
            shared_from_base<CLASS>(), sempr::core::EntityEventBase::CREATED  \
        );  \
        fireEvent(e);  \
    }

#define ENTITY_DEFAULT_LOADED(CLASS, BASE) \
    virtual void loaded_impl() override { \
        BASE::loaded_impl(); \
        auto e = std::make_shared<sempr::core::EntityEvent<CLASS> >(  \
            shared_from_base<CLASS>(), sempr::core::EntityEventBase::LOADED  \
        );  \
        fireEvent(e);  \
    }

#define ENTITY_DEFAULT_REMOVED(CLASS, BASE) \
    virtual void removed_impl() override { \
        BASE::removed_impl(); \
        auto e = std::make_shared<sempr::core::EntityEvent<CLASS> >(  \
            shared_from_base<CLASS>(), sempr::core::EntityEventBase::REMOVED  \
        );  \
        fireEvent(e);  \
    }

#define ENTITY_DEFAULT_EVENT_METHODS(class,base) \
    ENTITY_DEFAULT_CREATED(class,base) \
    ENTITY_DEFAULT_LOADED(class,base) \
    ENTITY_DEFAULT_CHANGED(class,base) \
    ENTITY_DEFAULT_REMOVED(class,base)


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
    using Event = core::EntityEvent<Entity>;

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
    */
    void created();
    void loaded();
    void removed();
    void added();

protected:
    /** fires an event **/
    void fireEvent(core::Event::Ptr e);

    template<class Derived>
    std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    /**
        Registers a child-entity: Every entity that is created inside of this
        and used in a relationship (aka, member variable smart pointer) has to
        be persisted before this one to prevent foreign-key-errors. Registered
        children are automatically handled inside pre- and postPersist of the
        Entity-Class. Also, this entity is set to be their parent, resulting in
        on-delete-cascade: If this is removed from the database, so are the
        children.
        TODO: Are events handled correctly in the case of the removal through
        a cascade?
    */
    void registerChildEntity(Entity::Ptr child);

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

    /**
        To make sure that a derived class does not forget to throw an
        EntityEvent<Entity> on change/creation/load/removed, the methods
        changed(), ... just call the _impl()-methods and check if
        Entity::..._impl() has been called, failing an assertion if not.
    */
    virtual void changed_impl();
    virtual void created_impl();
    virtual void loaded_impl();
    virtual void removed_impl();
    virtual void added_impl();

private:
    friend class odb::access;
    friend class core::Core;

    /** used to fire events, set by the core */
    #pragma db transient
    std::weak_ptr<core::EventBroker> broker_;

    /**
        In order to allow entities within entities and a correct
        on-delete-cascade mechanism (i.e., remove the child if the parent has
        been removed), we need to register children and handle them with care
        before persistence / update.
    */
    // no need to keep the references to the children.
    // Their "parent_" will be set, and we only
    // need to handle the children ONCE, i.e. when this entity is persisted
    // or updated.
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

    /** persist newly registered children upon persist/update */
    void handleChildrenPre(odb::database& db) const;
    /** set newly registered childrens parent to this and fire their
        created-events after persist/update */
    void handleChildrenPost(odb::database& db) const;

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
