#include "Component.hpp"
#include "Entity.hpp"
#include "Core.hpp"
#include "Exception.hpp"

namespace sempr {

Component::Component() : entity_(nullptr), tag_("")
{
}

Component::~Component()
{
}

const Entity* Component::getEntity() const
{
    return entity_;
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

void Component::loadFromJSON(cereal::JSONInputArchive& ar)
{
    ar(*this);
}


}



// define rete::util::to_string for Components
namespace rete { namespace util {

template <>
std::string to_string(const sempr::Component&)
{
    return "Component"; // wow. // TODO
}

template <>
std::string to_string(const sempr::Component::Ptr& c)
{
    if (!c) return "nullptr";
    return to_string(*c);
}

}}
