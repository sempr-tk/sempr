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
    // std::cout << "updateDatabase:" << '\n';
    // std::cout << "  changed: " << toUpdate_.size() << '\n';
    // std::cout << "  removed: " << toRemove_.size() << '\n';

    // save changed entities
    storage_->save(toUpdate_);
    toUpdate_.clear();

    // before removing, check the number of references!
    for (auto& obj : toRemove_) {
        // obj->removed() has already been called, so how many references to it are okay?

        // min 2 ref: toRemove_, session
        //      +1 as it may still live in user space
        //  or
        //      +2 if it is a child-entity: parent.someProperty, parent.children_
        //
        // so: 2 are best, 3 is unclear (user-space or module?), 4 is evil if it is not a child, but okay if it is,
        // and if any object has a reference count of 5 or higher there is probably something
        // going wrong.

        if (obj.use_count() > 4)
        {
            std::cerr   << "Warning: Removing object from the database which has a critical number "
                        << "of references left. Any unclean implementation of a processing module "
                        << "that didn't react to a removed-event? "
                        << "(" << obj->id() << ", " << obj.use_count() << ")" << '\n';
        }
    }

    // remove entities
    storage_->remove(toRemove_);
    toRemove_.clear();
}


}}
