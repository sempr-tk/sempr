#include "sempr/core/SimpleIncrementalIDGenerator.hpp"

sempr::core::SimpleIncrementalIDGenerator::SimpleIncrementalIDGenerator()
    : nextID_(0)
{
}

std::string sempr::core::SimpleIncrementalIDGenerator::createIDFor(entity::Entity::Ptr e)
{
    if (e->id().empty())
    {
        return "Entity_" + std::to_string(nextID_++);
    }
    return e->id();
}

void sempr::core::SimpleIncrementalIDGenerator::releaseIDOf(entity::Entity::Ptr)
{
    // dont care.
}
