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
    core::EntityEvent::Ptr e( 
        new core::EntityEvent(shared_from_this(), core::EntityEvent::CHANGED)
    );
    fireEvent(e);
}

} /* entity */
    
} /* sempr */
