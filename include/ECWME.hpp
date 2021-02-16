#ifndef SEMPR_CORE_ECTWME_HPP_
#define SEMPR_CORE_ECTWME_HPP_

#include <rete-core/TupleWME.hpp>

#include "Entity.hpp"
#include "Component.hpp"

#include <string>

namespace sempr {

/**
    The ECWME is the *only* SEMPR-specific WME that is added to the reasoner.
    It is a simple wrapper that combines an entity with a component and a tag.
    There will be different types of alpha nodes that differentiate between the
    types of components inside the ECWME.
*/
typedef rete::TupleWME<Entity::Ptr, Component::Ptr, std::string> ECWME;

}

#endif /* Include guard: SEMPR_CORE_ECTWME_HPP_ */


