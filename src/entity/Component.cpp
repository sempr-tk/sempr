#include "sempr/entity/Component.hpp"

namespace sempr { namespace entity {

Component::Component() : entity_(nullptr)
{
}

Component::~Component()
{
}


void Component::changed()
{
    // TODO
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
