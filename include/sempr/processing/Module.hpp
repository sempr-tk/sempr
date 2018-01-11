#ifndef SEMPR_PROCESSING_MODULE_H_
#define SEMPR_PROCESSING_MODULE_H_

#include <string>
#include <memory>
#include <sempr/core/Event.hpp>
#include <sempr/core/Observer.hpp>
#include <sempr/core/EntityEvent.hpp>
#include <sempr/query/Query.hpp>

namespace sempr { namespace processing {

class Module : public core::Observer{
public:
    using Ptr = std::shared_ptr<Module>;
    virtual std::string type() const;
    virtual ~Module(){}

    /**
        Called by the core in order to answer a given query.
        The base-implementation simply calls this->notify(q), allowing
        Modules to reuse the Observer-Observable-construction: Modules can add
        supported query-types through addOverload(..) just like in the event
        system. (Events and Queries both derive from Observable)
        Besides, Modules are allowed to override this method. The simple
        notify-call does have its limitations, as you need to register every
        single Query-type beforhand. E.g. the ActiveObjectStore has its own
        implementation of "answer".

        Note: I'm not sure if "observer-observable" is the right term here.
        In lack of better wording ... just stick with it for now.
    */
    virtual void answer(query::Query::Ptr q);
};

} /* processing */

} /* sempr */

#endif /* end of include guard: SEMPR_PROCESSING_MODULE_H_ */
