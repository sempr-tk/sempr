#ifndef SEMPR_CORE_ENTITYEVENT_H_
#define SEMPR_CORE_ENTITYEVENT_H_

#include <sempr/core/Event.h>
#include <sempr/entity/Entity.h>


namespace sempr { namespace core {
    
class EntityEvent : public Event {
public:
    using Ptr = std::shared_ptr<EntityEvent>;
    enum EventType { CREATED, CHANGED, REMOVED, LOADED };
    EntityEvent(entity::Entity::Ptr entity, EventType type);
    virtual ~EntityEvent();
    
    /** Access the entity this event refers to */
    entity::Entity::Ptr getEntity();
    
    /** convenience function including a dynamic pointer cast */
    template <class T>
    std::shared_ptr<T> getEntityAs() {
        return std::dynamic_pointer_cast<T>(entity_);
    }
    
    template <class T>
    bool entityIsA() const {
        return std::dynamic_pointer_cast<T>(entity_).get() != NULL;
    }
    
    /** returns the internal event type (created, changed, removed) */
    EventType what();

protected:
    entity::Entity::Ptr entity_;
    EventType type_;
};

    
} /* core */
    
} /* sempr */


#endif /* end of include guard: SEMPR_CORE_ENTITYEVENT_H_ */
