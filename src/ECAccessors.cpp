#include "ECAccessors.hpp"
#include "ECWME.hpp"

namespace sempr {

std::string EntityAccessor::toString() const
{
    return "Entity" + (index_ < 0 ? std::string("") : "[" + std::to_string(index_) + "]");
}

}
