#include "sempr/entity/Component.hpp"
#include "sempr/entity/Entity.hpp"
#include "sempr/core/Core.hpp"
#include "sempr/core/Exception.hpp"

namespace sempr { namespace entity {

Component::Component() : entity_(nullptr), tag_("")
{
}

Component::~Component()
{
}


void Component::setTag(const std::string& tag)
{
    if (entity_) throw sempr::Exception("Cannot set component tag after adding it to an entity");
    tag_ = tag;
}

std::string Component::getTag() const
{
    return tag_;
}


void Component::changed()
{
    // notify the core, if there is one
    if (entity_)
    {
        if (entity_->core())
        {
            entity_->core()->changedComponent(
                                entity_->shared_from_this(), 
                                this->shared_from_this()
            );
        }
    }
}


}}



// define rete::util::to_string for Components
namespace rete { namespace util {

template <>
std::string to_string(const sempr::entity::Component&)
{
    return "Component"; // wow. // TODO
}

template <>
std::string to_string(const sempr::entity::Component::Ptr& c)
{
    if (!c) return "nullptr";
    return to_string(*c);
}

}}
