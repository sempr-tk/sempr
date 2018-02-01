#include <sempr/processing/ActiveObjectStore.hpp>

namespace sempr { namespace processing {

ActiveObjectStore::ActiveObjectStore(storage::Storage::Ptr storage) : storage_(storage) {
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
        case EventType::CREATED:
            addNewEntity(e->getEntity());
        case EventType::LOADED:
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


void ActiveObjectStore::answer(query::Query::Ptr query)
{
    auto q = std::dynamic_pointer_cast<query::ObjectQueryBase>(query);
    if (q) {
        answer(q);
    }
}

void ActiveObjectStore::answer(query::ObjectQueryBase::Ptr query)
{
    // since we don't have any indexing right now (besides the id->ptr map)
    // we just need to traverse all items and apply the user-defined criteria.
    // -- how could we get any indexing into the queries, anyway? If the
    // module knew how to answer a query all alone, no problem, but in this case
    // the query contains important logic: it knows the type of the object it
    // is looking for, and may use type-specific information as a selection
    // criterion
    for (auto e : entities_) {
        query->consider(e.second);
        // TODO: if we make consider() thread-safe by splitting the checks from
        // the insertion into the resultset we can use e.g.
        // #pragma omp parallel for
        // to speed this up. (who knows how expensive the checks might be?)
    }
}

void ActiveObjectStore::persistEntities(){

  for (auto o : newEntities_){
    //std::cout << "send to persist: " << o->id() << std::endl;
    storage_->save(o);
  }
  newEntities_.clear();
}

void ActiveObjectStore::persistEntitiesInBulk(){
  storage_->save(newEntities_);
  newEntities_.clear();
}

void ActiveObjectStore::addNewEntity(entity::Entity::Ptr e){
  if (e.get()) {
      newEntities_.push_back(e);
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
        // remove from newEntities
        for (auto it = newEntities_.begin(); it != newEntities_.end(); it++) {
          if((*it)->id() == e->id()) {
            newEntities_.erase(it);
            break;
          }
        }

        // remove from entities
        auto it = entities_.find(e->id());
        if (it != entities_.end()) {
            entities_.erase(it);
        }
    }
}

} /* processing */

} /* sempr */
