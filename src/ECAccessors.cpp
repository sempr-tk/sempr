#include "ECAccessors.hpp"
#include "ECWME.hpp"
#include "RDF.hpp"

namespace sempr {

EntityAccessor::EntityAccessor()
{
}

std::string EntityAccessor::toString() const
{
    return "Entity" + (index_ < 0 ? std::string("") : "[" + std::to_string(index_) + "]");
}

EntityAccessor* EntityAccessor::clone() const
{
    auto a = new EntityAccessor();
    a->index_ = index_;
    return a;
}


void EntityAccessor::getValue(ECWME::Ptr wme, Entity::Ptr& value) const
{
    value = std::get<0>(wme->value_);
}

void EntityAccessor::getValue(ECWME::Ptr wme, std::string& value) const
{
    value = std::get<0>(wme->value_)->id();
}

void EntityAccessor::getValue(ECWME::Ptr wme, rete::TriplePart& value) const
{
    value.value = "<" + sempr::baseURI() + std::get<0>(wme->value_)->id() + ">";
}

bool EntityAccessor::equals(const AccessorBase& other) const
{
    auto o = dynamic_cast<const EntityAccessor*>(&other);
    if (o) return true;
    return false;
}



bool ComponentTagAccessor::equals(const rete::AccessorBase& other) const
{
    auto o = dynamic_cast<const ComponentTagAccessor*>(&other);
    if(o) return true;
    return false;
}

void ComponentTagAccessor::getValue(ECWME::Ptr wme, std::string& value) const
{
    auto component = std::get<1>(wme->value_);
    value = component->getTag();
}

ComponentTagAccessor* ComponentTagAccessor::clone() const
{
    auto a = new ComponentTagAccessor();
    a->index_ = index_;
    return a;
}

std::string ComponentTagAccessor::toString() const
{
    return "Component" +
           (index_ < 0 ? std::string("") : "[" + std::to_string(index_) + "]") +
           ".tag";
}

}
