#include "Core.hpp"
#include "ECWME.hpp"
#include "Component.hpp"
#include "Exception.hpp"
#include "SimpleIncrementalIDGenerator.hpp"

#include <rete-reasoner/AssertedEvidence.hpp>

namespace sempr {

Core::Core(IDGenerator::Ptr idgen, Storage::Ptr storage)
    : idgen_(idgen), storage_(storage)
{
}

Core::Core(IDGenerator::Ptr idgen)
    : Core(idgen, nullptr)
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

std::mutex& Core::reasonerMutex()
{
    return reasonerMutex_;
}

rete::Reasoner& Core::reasoner()
{
    return reasoner_;
}


rete::RuleParser& Core::parser()
{
    return ruleParser_;
}

std::vector<size_t> Core::addRules(const std::string& ruleString)
{
    std::lock_guard<std::mutex> lg(reasonerMutex_);
    // NOTE: The lock_guard may cause problems, if addRules is called from
    // within performInference as an effect of a rule! I think a recursive_mutex
    // would be a fix for that? TODO!

    auto newRules = ruleParser_.parseRules(ruleString, reasoner_.net());

    std::vector<size_t> ids;
    for (auto r : newRules)
    {
        // for the result
        ids.push_back(r->id());

        // but also, store the rule permanently
        rules_.push_back(r);
    }
    return ids;
}

void Core::removeRule(size_t id)
{
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [id](rete::ParsedRule::Ptr& rule) -> bool
        {
            return rule->id() == id;
        });

    if (it != rules_.end())
    {
        (*it)->disconnect();
        rules_.erase(it);
    }
}

std::vector<rete::ParsedRule::Ptr> Core::rules()
{
    return rules_;
}


void Core::performInference()
{
    std::lock_guard<std::mutex> lock(reasonerMutex_);
    reasoner_.performInference();
}

Storage::Ptr Core::storage()
{
    return storage_;
}

void Core::addEntity(Entity::Ptr entity)
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

    // Persist the entity
    if (storage_) storage_->save(entity);

    // add the WMEs to the reasoner
    auto components = entity->getComponents<Component>();
    for (auto& c : components)
    {
        addedComponent(entity, c);
    }
}


Entity::Ptr Core::getEntity(const std::string& id)
{
    for (auto e : entities_)
    {
        if (e->id() == id) return e;
    }
    return nullptr;
}


void Core::removeEntity(Entity::Ptr entity)
{
    if (entity->core_ != this)
    {
        throw sempr::Exception("Entity not part of this Core");
    }

    // Un-persist entity
    if (storage_) storage_->remove(entity);

    // remove corresponding WMEs
    auto evidence = std::make_shared<rete::AssertedEvidence>(entity->id());

    {
        std::lock_guard<std::mutex> lock(reasonerMutex_);
        reasoner_.removeEvidence(evidence);
    }

    this->entities_.erase(entity);
    entity->core_ = nullptr;
}

void Core::addedComponent(Entity::Ptr entity,
                          Component::Ptr component)
{
    // update storage
    if (storage_) storage_->save(entity, component);

    auto evidence = std::make_shared<rete::AssertedEvidence>(entity->id());
    auto wme = std::make_shared<ECWME>(entity, component);

    {
        std::lock_guard<std::mutex> lock(reasonerMutex_);
        reasoner_.addEvidence(wme, evidence);
    }
}

void Core::removedComponent(Entity::Ptr entity,
                          Component::Ptr component)
{
    // Un-persist component
    if (storage_) storage_->remove(entity, component);

    auto evidence = std::make_shared<rete::AssertedEvidence>(entity->id());
    auto wme = std::make_shared<ECWME>(entity, component);

    {
        std::lock_guard<std::mutex> lock(reasonerMutex_);
        reasoner_.removeEvidence(wme, evidence);
    }
}

void Core::changedComponent(Entity::Ptr entity,
                          Component::Ptr component)
{
    // update storage
    if (storage_) storage_->save(entity, component);

    // NOTE: You may have noticed that there are often new evidences and WMEs
    // created, even for "removedComponent" and "changedComponent". This is okay
    // since the WMEs evaluate to be identical (as they point to the same
    // entities and components), and the rete engine handles this case in the
    // AlphaMemories by propagating the pointer to the WME that was first
    // added, and discards the new wme. The reasoner also compares WMEs by value
    // to see if they are backed by evidence etc., so we should be good.

    auto wme = std::make_shared<ECWME>(entity, component);
    {
        std::lock_guard<std::mutex> lock(reasonerMutex_);
        reasoner_.net().getRoot()->activate(wme, rete::PropagationFlag::UPDATE);
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
//
//
//void Core::addModule(processing::ModuleBase::Ptr module) {
//    modules_.push_back(module);
//    module->core_ = this;   // allow the module to ask queries later on.
//    // NOTE: If we ever implement a "removeModule" make sure to un-set the pointer.
//
//    eventBroker_->addObserver(module);
//}

} /* sempr */
