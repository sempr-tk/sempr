#include <sempr/processing/DBUpdateModule.hpp>
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
    case EventType::ADDED:
        addEntity(e->getEntity());
        break;
    case EventType::CHANGED:
        storage_->save(e->getEntity());
        break;
    case EventType::REMOVED:
        removeEntity(e->getEntity());
        break;
    default:
    // nothing
        break;
  }
}

void DBUpdateModule::addEntity(entity::Entity::Ptr e){
  if (e.get()) {
      newEntities_.push_back(e);
  }
}

void DBUpdateModule::persistEntities(){
  std::cout << "persisting " << newEntities_.size() << " entities:" << std::endl;

  for (auto o : newEntities_){
    //std::cout << "send to persist: " << o->id() << std::endl;
    storage_->save(o);
  }
  newEntities_.clear();
}

void DBUpdateModule::persistEntitiesInBulk(){
  std::cout << "persisting " << newEntities_.size() << " entities in bulk:" << std::endl;

  storage_->save(newEntities_);
  newEntities_.clear();
}

void DBUpdateModule::removeEntity(entity::Entity::Ptr e){
  // remove from newEntities
  for (auto it = newEntities_.begin(); it != newEntities_.end(); it++) {
    if((*it)->id() == e->id()) {
      newEntities_.erase(it);
      break;
    }
  }
}

}}
