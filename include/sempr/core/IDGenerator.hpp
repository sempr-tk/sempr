#ifndef SEMPR_CORE_IDGENERATOR_HPP_
#define SEMPR_CORE_IDGENERATOR_HPP_

#include "sempr/entity/Entity.hpp"
#include <string>

namespace sempr { namespace core {

/**
    Interface for ID generation strategies.
    Gets an entity, returns an ID for it.
*/
class IDGenerator {
public:
    virtual ~IDGenerator() = default;
    using Ptr = std::shared_ptr<IDGenerator>;

    /**
        Reserves a new ID for the given entity.
    */
    virtual std::string createIDFor(entity::Entity::Ptr) = 0;

    /**
        Signal to the IDGenerator that the entity will be removed and the
        ID may be re-used.
    */
    virtual void releaseIDOf(entity::Entity::Ptr) = 0;
};

}}

#endif /* include guard: SEMPR_CORE_IDGENERATOR_HPP_ */

