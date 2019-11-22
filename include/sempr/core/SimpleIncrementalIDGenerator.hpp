#ifndef SEMPR_CORE_SIMPLEINCREMENTALIDGENERATOR_HPP_
#define SEMPR_CORE_SIMPLEINCREMENTALIDGENERATOR_HPP_

#include "IDGenerator.hpp"

namespace sempr { namespace core {

/**
    IDGenerator that gives every entity a new numeric ID.
    Always starts counting from 0.

    DO NOT USE FOR PERSISTING ENTITIES.
*/
class SimpleIncrementalIDGenerator : public IDGenerator {
    size_t nextID_;
public:
    SimpleIncrementalIDGenerator();
    std::string createIDFor(entity::Entity::Ptr) override;
    void releaseIDOf(entity::Entity::Ptr) override;
};


}}

#endif /* SEMPR_CORE_SIMPLEINCREMENTALIDGENERATOR_HPP_ */
