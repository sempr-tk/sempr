#ifndef SEMPR_ENTITY_ENTITY_H_
#define SEMPR_ENTITY_ENTITY_H_

#include <odb/core.hxx>

#include <string>
#include <vector>
#include <memory>

#include <boost/uuid/uuid_io.hpp>
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
    Entity(){}
    using Ptr = std::shared_ptr<Entity>;
    using Event = core::EntityEvent<Entity>;
    virtual std::string id() const { return boost::uuids::to_string(uuid()); }
    virtual ~Entity(){}

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
    
protected:
    /** fires an event **/
    void fireEvent(core::Event::Ptr e);
    
    template<class Derived>
    std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }
    
    
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
    
private:
    friend class odb::access;
    friend class core::Core;
    
    /** used to fire events, set by the core */
    #pragma db transient
    std::weak_ptr<core::EventBroker> broker_;
    
    /**
        Helper to check if the base implementations of ..._impl() have been
        called. 
    */
    #pragma db transient
    bool baseCalled_;
};

}}

#endif /* end of include guard: SEEPRO_ENTITY_ENTITY_H_ */
