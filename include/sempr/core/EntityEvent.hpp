#ifndef SEMPR_CORE_ENTITYEVENT_H_
#define SEMPR_CORE_ENTITYEVENT_H_

#include <sempr/core/Event.hpp>

namespace sempr { namespace core {
    
class EntityEventBase : public Event {
public:
    enum EventType { CREATED, CHANGED, REMOVED, LOADED };
    EntityEventBase(EventType t) : type_(t) {}
    virtual ~EntityEventBase(){}
    
    /** returns the internal event type (created, changed, removed) */
    EventType what() { return type_; }
    
protected:
    EventType type_;
    
};

/**
    The templated class EntityEvent adds static type-safety to the type of
    entity it is pointing to. Additionally, as each instantiaton is a new type,
    it can be used to trigger only those processing-modules that explicitly
    subscribed to them. 
    TODO: Caveat: Subscribing to BaseEntityEvent is not sufficient
    to get all events -- whenever a specific event is fired, a BaseEntityEvent
    should be fired, too!
*/
template<class EntityT>    
class EntityEvent : public EntityEventBase {
public:
    using Ptr = std::shared_ptr<EntityEvent<EntityT> >;
    EntityEvent(std::shared_ptr<EntityT> entity, EventType type)
        : entity_(entity), EntityEventBase(type)
    {
    }
    
    virtual ~EntityEvent(){}
    
    /** Access the entity this event refers to */
    std::shared_ptr<EntityT> getEntity() {
        return std::static_pointer_cast<EntityT>(entity_);
    }
    
protected:
    std::shared_ptr<EntityT> entity_;    
};

    
} /* core */
    
} /* sempr */


#endif /* end of include guard: SEMPR_CORE_ENTITYEVENT_H_ */
