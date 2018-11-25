#ifndef SEMPR_PROCESSING_MODULEBASE_HPP_
#define SEMPR_PROCESSING_MODULEBASE_HPP_

// TODO merge Observer and ModuleBase?
#include <sempr/core/Observer.hpp>
#include <sempr/query/Query.hpp>

namespace sempr {

namespace core {
    class Core; // forward declaration: Core uses ModuleBase!
} /* core */

namespace processing {



/**
    Base class for Modules: Stores a pointer to the core and thus enables modules to
    ask queries.
*/
class ModuleBase : public core::Observer {
    core::Core* core_; // set by the core upon adding the module
    friend class core::Core;
protected:
    /**
     * Allows modules to ask queries.
     */
    void ask(query::Query::Ptr q);

    /**
     * Return the pointer to the registered core
     */
    core::Core* core();

public:
    using Ptr = std::shared_ptr<ModuleBase>;

    virtual std::string type() const;
    virtual ~ModuleBase(){}

};



} /* processing */
} /* sempr */

#endif /* end of include guard: SEMPR_PROCESSING_MODULEBASE_HPP_ */
