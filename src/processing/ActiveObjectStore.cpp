#include <sempr/processing/ActiveObjectStore.hpp>

namespace sempr { namespace processing {

ActiveObjectStore::ActiveObjectStore()
{
    // nothing to do.
}

ActiveObjectStore::~ActiveObjectStore()
{
}

std::string ActiveObjectStore::type() const {
    return "ActiveObjectStore";
}

void ActiveObjectStore::process(core::EntityEvent<entity::Entity>::Ptr e)
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



void ActiveObjectStore::process(query::ObjectQueryBase::Ptr query)
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
