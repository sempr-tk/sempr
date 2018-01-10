#include <sempr/processing/ActiveObjectStore.hpp>

namespace sempr { namespace processing {

ActiveObjectStore::ActiveObjectStore() {
    addOverload<entity::Entity::Event>(
        [this](entity::Entity::Event::Ptr e) { process(e); }
    );
}

ActiveObjectStore::~ActiveObjectStore()
{
}

std::string ActiveObjectStore::type() const {
    return "ActiveObjectStore";
}

void ActiveObjectStore::process(entity::Entity::Event::Ptr e)
{
    typedef core::EntityEventBase::EventType EventType;

    switch(e->what()) {
        case EventType::LOADED:
        case EventType::CREATED:
            addEntity(e->getEntity());
            break;
        case EventType::REMOVED:
            removeEntity(e->getEntity());
            break;
        default:
        // nothing
            break;
    }
}

void ActiveObjectStore::addEntity(entity::Entity::Ptr e)
{
    if (e.get()) {
        entities_[e->id()] = e;
    }
}

void ActiveObjectStore::removeEntity(entity::Entity::Ptr e)
{
    if (e.get()) {
        auto it = entities_.find(e->id());
        if (it != entities_.end()) {
            entities_.erase(it);
        }
    }
}

} /* processing */

} /* sempr */
