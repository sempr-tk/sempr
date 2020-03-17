#ifndef SEMPR_CORE_STORAGE_HPP_
#define SEMPR_CORE_STORAGE_HPP_

#include "Entity.hpp"
#include "Component.hpp"

namespace sempr {



/**
    Interface for persistance strategies.
    So far, only saving entities is required by the interface. The sempr::Core
    will not decide on its own what to load -- this has to be done by the user
    when initializing the system.
*/
class Storage {
public:
    virtual ~Storage() = default;
    using Ptr = std::shared_ptr<Storage>;

    /**
        Informs the storage that the given entity should be persisted.
        Called the first time a entity is added to the system.
    */
    virtual void save(Entity::Ptr) = 0;

    /**
        Informs the storage that a specific component inside an entity has
        been changed.
    */
    virtual void save(Entity::Ptr, Component::Ptr) = 0;
};


}

#endif /* include guard: SEMPR_CORE_STORAGE_HPP_ */

