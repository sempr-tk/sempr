#include "sempr/core/EntityAccessor.hpp"
#include "sempr/core/RDF.hpp"
#include <typeinfo>

using namespace sempr;
using namespace sempr::core;


void EntityIDAccessor::getValue(rete::WME::Ptr wme, std::string& str) const
{
    auto ewme = std::static_pointer_cast<EntityWME>(wme);
    str = "<" + sempr::baseURI() + ewme->entity()->id() + ">";
}

std::string EntityIDAccessor::toString() const
{
    return "EntityID" + (index_ < 0 ? std::string("") : "[" + std::to_string(index_) + "]");
}

EntityIDAccessor* EntityIDAccessor::clone() const
{
    return new EntityIDAccessor(*this);
}

bool EntityIDAccessor::equals(const rete::Accessor& other) const
{
    // all information in the type.
    return typeid(*this) == typeid(other);
}

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

EntityAccessorBase::EntityAccessorBase()
{
    registerType<EntityAccessorBase>();
}


void EntityAccessorBase::getValue(rete::WME::Ptr wme, entity::Entity::Ptr& entity) const
{
    auto ewme = std::static_pointer_cast<EntityWME>(wme);
    entity = ewme->entity();
}

bool EntityAccessorBase::equals(const rete::Accessor& other) const
{
    // only the type specific checks -- the operator == already checks for identity and index.
    return typeid(*this) == typeid(other);
}

bool EntityAccessorBase::canCompareValues(const rete::Accessor& other) const
{
    // EntityAccessors must never be compared to anything!
    // This way we prevent invalid Accessor-overriding in the RuleParser.
    // see: https://git.hb.dfki.de/nniemann/rete/issues/6
    return false;
}

bool EntityAccessorBase::valuesEqual(rete::Accessor& other, rete::Token::Ptr token, rete::WME::Ptr wme)
{
    throw std::exception();
}

EntityAccessorBase* EntityAccessorBase::clone() const
{
    return new EntityAccessorBase(*this);
}

std::string EntityAccessorBase::toString() const
{
    // "Entity" if for a wme, "Entity[n]" if accessing a token
    return "Entity" + (index_ < 0 ? std::string("") : "[" + std::to_string(index_) + "]");
}
