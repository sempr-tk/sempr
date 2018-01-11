#include <sempr/processing/Module.hpp>

namespace sempr { namespace processing {

std::string Module::type() const {
    return "Module";
}

void Module::answer(query::Query::Ptr q)
{
    notify(q);
}


} /* processing */

} /* sempr */
