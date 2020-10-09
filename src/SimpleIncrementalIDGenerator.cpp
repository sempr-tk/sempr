#include "SimpleIncrementalIDGenerator.hpp"

sempr::SimpleIncrementalIDGenerator::SimpleIncrementalIDGenerator()
    : nextID_(0)
{
}

std::string sempr::SimpleIncrementalIDGenerator::createIDFor(Entity::Ptr e)
{
    if (e->id().empty())
    {
        return "Entity_" + std::to_string(nextID_++);
    }
    return e->id();
}

void sempr::SimpleIncrementalIDGenerator::releaseIDOf(Entity::Ptr)
{
    // dont care.
}
