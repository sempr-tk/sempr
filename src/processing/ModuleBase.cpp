#include <sempr/processing/ModuleBase.hpp>
#include <sempr/core/Core.hpp>

namespace sempr { namespace processing {

std::string ModuleBase::type() const {
    return "ModuleBase";
}

void ModuleBase::ask(query::Query::Ptr q)
{
    if (this->core_) { this->core_->answerQuery(q); }
}

} /* processing */

} /* sempr */
