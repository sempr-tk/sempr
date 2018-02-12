#include <sempr/processing/DBUpdateModule.hpp>
#include <sempr/core/Exception.hpp>
#include <iostream>

namespace sempr { namespace processing {

DBUpdateModule::DBUpdateModule(storage::Storage::Ptr storage)
    : storage_(storage)
{
    addOverload<entity::Entity::Event>(
        [this](entity::Entity::Event::Ptr e) {
            process(e);
        }
    );
}

void DBUpdateModule::process(entity::Entity::Event::Ptr e)
{
  typedef core::EntityEventBase::EventType EventType;

  switch(e->what()) {
    case EventType::CREATED: // created: persist immediately!
        registerAdded(e->getEntity());
        break;
    case EventType::CHANGED: // register to save it later
        registerChanged(e->getEntity());
        break;
    case EventType::REMOVED:
        registerRemoved(e->getEntity());
        break;
    default:
    // nothing
        break;
  }
}


void DBUpdateModule::registerAdded(storage::DBObject::Ptr obj)
{
    // evil stuff: is it already flagged for removal?
    if (std::find(toRemove_.begin(), toRemove_.end(), obj) != toRemove_.end())
    {
        throw sempr::Exception(
            "registerAdded called for " + obj->id() +
            ", which is already flagged for removal! (won't save.)"
        );
    }

    storage_->save(obj);
}

void DBUpdateModule::registerChanged(storage::DBObject::Ptr obj)
{
    // evil stuff: is it already flagged for removal?
    if (std::find(toRemove_.begin(), toRemove_.end(), obj) != toRemove_.end())
    {
        throw sempr::Exception(
            "registerChanged called for " + obj->id() +
            ", which is already flagged for removal! (won't save.)"
        );
    }

    // only if it isn't already registered
    if (std::find(toUpdate_.begin(), toUpdate_.end(), obj) == toUpdate_.end())
    {
        toUpdate_.push_back(obj);
    }
}

void DBUpdateModule::registerRemoved(storage::DBObject::Ptr obj)
{
    // only if not already registered
    if (std::find(toRemove_.begin(), toRemove_.end(), obj) == toRemove_.end())
    {
        toRemove_.push_back(obj);
    }

    // if it is registered as changed, remove it from the list.
    // no need to save it if we remove it afterwards
    auto it = std::find(toUpdate_.begin(), toUpdate_.end(), obj);
    if (it != toUpdate_.end())
    {
        toUpdate_.erase(it);
    }
}


void DBUpdateModule::updateDatabase()
{
    // save changed entities
    storage_->save(toUpdate_);
    toUpdate_.clear();

    // remove entities
    storage_->remove(toRemove_);
    toRemove_.clear();
}

void DBUpdateModule::persistEntities() {
  std::cout << "persisting " << toUpdate_.size() << " entities:" << std::endl;

  for (auto o : toUpdate_){
    //std::cout << "send to persist: " << o->id() << std::endl;
    storage_->save(o);
  }
  toUpdate_.clear();

  std::cout << "removing " << toRemove_.size() << " entities" << std::endl;

  for (auto o : toRemove_) {
      storage_->remove(o);
  }
  toRemove_.clear();
}

void DBUpdateModule::persistEntitiesInBulk(){
  std::cout << "persisting " << toUpdate_.size() << " entities in bulk," << std::endl;
  std::cout << " and removing " << toRemove_.size() << std::endl;
  updateDatabase();
}



}}
