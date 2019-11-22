#include "sempr/core/Core.hpp"
#include "sempr/core/ECTWME.hpp"
#include "sempr/entity/Component.hpp"
#include "sempr/core/Exception.hpp"
#include "sempr/core/SimpleIncrementalIDGenerator.hpp"

#include <rete-reasoner/AssertedEvidence.hpp>

namespace sempr { namespace core {

Core::Core(IDGenerator::Ptr idgen)
    : idgen_(idgen)
{
}

Core::Core() : Core(std::make_shared<SimpleIncrementalIDGenerator>())
{
}

Core::~Core()
{
    for (auto entity : entities_)
    {
        entity->core_ = nullptr;
    }
}

void Core::addEntity(entity::Entity::Ptr entity)
{
    // add it to the core if the entity wasn't added to one before
    if (entity->core_)
    {
        throw sempr::Exception("Entity already present in a Core!");
    }
    entity->core_ = this;
    this->entities_.insert(entity);


    // assign an ID if the entity does not have one, yet
    if (entity->id().empty())
    {
        entity->setId(idgen_->createIDFor(entity));
    }

    // TODO: Persist the entity
    
    // Create an "evidence" for the WMEs to be added.
    auto evidence = std::make_shared<rete::AssertedEvidence>(entity->id());

    auto components = entity->getComponents<entity::Component>();
    for (auto& c : components)
    {
        auto wme = std::make_shared<ECTWME>(entity, c.first, c.second);

        // add the fact to the reasoner
        reasoner_.addEvidence(wme, evidence);
    }
}


//void Core::answerQuery(query::Query::Ptr q)
//{
//    // same procedure as with events:
//    for (auto m : modules_) {
//        // m->answer(q);
//        m->doProcess(q);
//    }
//}
//void Core::removeEntity(entity::Entity::Ptr entity) {
//    if (entity.get()) {
//        entity->removed();
//    }
//}
//
//
//void Core::addModule(processing::ModuleBase::Ptr module) {
//    modules_.push_back(module);
//    module->core_ = this;   // allow the module to ask queries later on.
//    // NOTE: If we ever implement a "removeModule" make sure to un-set the pointer.
//
//    eventBroker_->addObserver(module);
//}

} /* core */

} /* sempr */
