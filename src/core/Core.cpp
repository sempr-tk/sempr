#include <sempr/core/Core.hpp>
#include <sempr/core/EntityEvent.hpp>

namespace sempr { namespace core {

Core::Core(storage::Storage::Ptr backend)
    : storage_(backend)
{
    eventBroker_ = EventBroker::getInstance();
}

void Core::answerQuery(query::Query::Ptr q)
{
    // same procedure as with events:
    for (auto m : modules_) {
        // m->answer(q);
        m->doProcess(q);
    }
}

void Core::addEntity(entity::Entity::Ptr entity) {
    if (entity.get()) {
        entities_[entity->id()] = entity;

        // persist
        storage_->save(entity);

        // signal update
        entity->created();
        // auto ev = std::make_shared<EntityEvent<entity::Entity> >(entity, EntityEventBase::CREATED);
        // eventBroker_->enqueueEvent(ev);
    }
}

void Core::removeEntity(entity::Entity::Ptr entity) {
    if (entity.get()) {
        storage_->remove(entity);
        entity->removed();
    }
}

// void Core::removeEntity(const std::string& entity_id) {
//     auto it = entities_.find(entity_id);
//     if (it != entities_.end()) {
//         storage_->remove(it->second);
//         entities_.erase(it);
//     }
// }

void Core::addModule(processing::ModuleBase::Ptr module) {
    modules_.push_back(module);
    module->core_ = this;   // allow the module to ask queries later on.
    // NOTE: If we ever implement a "removeModule" make sure to un-set the pointer.

    eventBroker_->addObserver(module);
}

} /* core */

} /* sempr */
