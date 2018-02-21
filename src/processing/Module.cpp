#include <sempr/processing/Module.hpp>
#include <sempr/core/Core.hpp>

namespace sempr { namespace processing {

std::string Module::type() const {
    return "Module";
}

void Module::answer(query::Query::Ptr q)
{
    notify(q);
}

void Module::ask(query::Query::Ptr q)
{
    if (this->core_) { this->core_->answerQuery(q); }
}


} /* processing */

} /* sempr */
