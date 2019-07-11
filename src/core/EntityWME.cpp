#include "sempr/core/EntityWME.hpp"

using namespace sempr::core;
using sempr::entity::Entity;

EntityWME::EntityWME(Entity::Ptr entity) : entity_(entity)
{
}

Entity::Ptr EntityWME::entity() const
{
    return entity_;
}

std::string EntityWME::toString() const
{
    return entity_->id();
}

bool EntityWME::operator<(const rete::WME& other) const
{
    if (auto o = dynamic_cast<const EntityWME*>(&other))
    {
        return entity_->id() < o->entity_->id();
    }
    return true; // Should never happen.
}
