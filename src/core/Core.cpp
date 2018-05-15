#include <sempr/core/Core.hpp>
#include <sempr/core/EntityEvent.hpp>

namespace sempr { namespace core {

Core::Core()
{
    eventBroker_ = EventBroker::getInstance();
}

void Core::answerQuery(query::Query::Ptr q)
{
    // same procedure as with events:
    for (auto m : modules_) {
        m->answer(q);
    }
}

void Core::addEntity(entity::Entity::Ptr entity) {
    if (entity.get()) {
        // signal update
        entity->created();
    }
}

void Core::removeEntity(entity::Entity::Ptr entity) {
    if (entity.get()) {
        entity->removed();
    }
}


void Core::addModule(processing::Module::Ptr module) {
    modules_.push_back(module);
    module->core_ = this;   // allow the module to ask queries later on.
    // NOTE: If we ever implement a "removeModule" make sure to un-set the pointer.

    eventBroker_->addObserver(module);
}

} /* core */

} /* sempr */
