#include "ECAccessors.hpp"
#include "ECWME.hpp"

namespace sempr {

EntityAccessor::EntityAccessor()
{
    registerType<EntityAccessor>();
}

std::string EntityAccessor::toString() const
{
    return "Entity" + (index_ < 0 ? std::string("") : "[" + std::to_string(index_) + "]");
}

EntityAccessor* EntityAccessor::clone() const
{
    return new EntityAccessor(*this);
}


void EntityAccessor::getValue(rete::WME::Ptr wme, Entity::Ptr& value) const
{
    auto ec = std::static_pointer_cast<ECWME>(wme);
    value = std::get<0>(ec->value_);
}

void EntityAccessor::getValue(rete::WME::Ptr wme, std::string& value) const
{
    auto ec = std::static_pointer_cast<ECWME>(wme);
    value = std::get<0>(ec->value_)->id();
}

bool EntityAccessor::equals(const Accessor& other) const
{
    auto o = dynamic_cast<const EntityAccessor*>(&other);
    if (o) return true;
    return false;
}



bool ComponentTagAccessor::equals(const rete::Accessor& other) const
{
    auto o = dynamic_cast<const ComponentTagAccessor*>(&other);
    if(o) return true;
    return false;
}

void ComponentTagAccessor::getValue(rete::WME::Ptr wme, std::string& value) const
{
    auto ecwme = std::static_pointer_cast<ECWME>(wme);
    auto component = std::get<1>(ecwme->value_);
    value = component->getTag();
}

ComponentTagAccessor* ComponentTagAccessor::clone() const
{
    return new ComponentTagAccessor(*this);
}

}
