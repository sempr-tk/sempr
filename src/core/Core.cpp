#include <sempr/core/Core.h>
#include <sempr/core/EntityEvent.h>

namespace sempr { namespace core {

Core::Core(storage::Storage::Ptr backend)
    : storage_(backend), eventBroker_(new EventBroker())
{
}

void Core::addEntity(entity::Entity::Ptr entity) {
    if (entity.get()) {
        entities_[entity->id()] = entity;
        entity->broker_ = getEventBroker();
        
        // persist
        storage_->save(entity);
        
        // signal update
        auto ev = std::make_shared<EntityEvent>(entity, EntityEvent::CREATED);
        eventBroker_->enqueueEvent(ev);
    }
}

void Core::removeEntity(entity::Entity::Ptr entity) {
    if (entity.get()) {
        removeEntity(entity->id());
    }
}

void Core::removeEntity(const std::string& entity_id) {
    auto it = entities_.find(entity_id);
    if (it != entities_.end()) {
        storage_->remove(it->second);
        entities_.erase(it);
    }
}

void Core::addModule(processing::Module::Ptr module) {
    modules_.push_back(module);
    eventBroker_->addObserver(module);
}

} /* core */
    
} /* sempr */
