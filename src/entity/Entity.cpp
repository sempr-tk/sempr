#include "sempr/core/Exception.hpp"
#include "sempr/entity/Entity.hpp"
#include <cassert>
//#include <Entity_odb.h>

namespace sempr { namespace entity {

/**
    Default: use IDGen<Entity>
*/
Entity::Entity() : core_(nullptr), id_("")
{
}

std::string Entity::id() const
{
    return id_;
}

void Entity::setId(const std::string& id)
{
    if (!id_.empty()) throw sempr::Exception("Entity already has an id");
    id_ = id;
}


} /* entity */
} /* sempr */


#include <rete-core/Util.hpp>

namespace rete { namespace util {

template <> 
std::string to_string(const sempr::entity::Entity& e)
{
    return e.id();
}

template <>
std::string to_string(const sempr::entity::Entity::Ptr& e)
{
    if (!e) return "nullptr";
    return to_string(*e);
}

}}

