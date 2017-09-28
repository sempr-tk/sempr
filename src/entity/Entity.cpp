#include <sempr/entity/Entity.h>
#include <sempr/core/EntityEvent.h>
#include <sempr/core/EventBroker.h>

namespace sempr { namespace entity {

void Entity::fireEvent(core::Event::Ptr e) {
    core::EventBroker::Ptr b = broker_.lock();
    if (b) {
        b->enqueueEvent(e);
    }
}

void Entity::changed() {
    Event::Ptr e = std::make_shared<Event>(shared_from_this(), Event::CHANGED);
    fireEvent(e);
}

void Entity::created() {
    Event::Ptr e = std::make_shared<Event>(shared_from_this(), Event::CREATED);
    fireEvent(e);
}

void Entity::loaded() {
    Event::Ptr e = std::make_shared<Event>(shared_from_this(), Event::LOADED);
    fireEvent(e);
}

void Entity::removed() {
    Event::Ptr e = std::make_shared<Event>(shared_from_this(), Event::REMOVED);
    fireEvent(e);
}

} /* entity */
    
} /* sempr */
