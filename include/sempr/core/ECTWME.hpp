#ifndef SEMPR_CORE_ECTWME_HPP_
#define SEMPR_CORE_ECTWME_HPP_

#include "sempr/entity/Entity.hpp"
#include "sempr/entity/Component.hpp"
#include <rete-core/TupleWME.hpp>

#include <string>

namespace sempr { namespace core {

/**
    The ECTWME is the *only* SEMPR-specific WME that is added to the reasoner.
    It is a simple wrapper that combines an entity with a component and a tag.
    There will be different types of alpha nodes that differentiate between the
    types of components inside the ECTWME.
*/
typedef rete::TupleWME<entity::Entity::Ptr, entity::Component::Ptr, std::string> ECTWME;

}}

#endif /* Include guard: SEMPR_CORE_ECTWME_HPP_ */


