#ifndef SEMPR_PROCESSING_MODULE_H_
#define SEMPR_PROCESSING_MODULE_H_

#include <string>
#include <memory>
#include <sempr/core/Event.hpp>
#include <sempr/core/Observable.hpp>
#include <sempr/core/Observer.hpp>
#include <sempr/core/EntityEvent.hpp>
#include <sempr/query/Query.hpp>
#include <sempr/processing/ModuleBase.hpp>

namespace sempr {

namespace core {
    class Core; // forward declaration of Core, since the module will keep a ptr to it.
}

namespace processing {

/**
    The Module-template is just an abstraction layer from the Observer: For every given subclass of
    Observable it adds a virtual method that gets called when an instance of such type is received.

    Use it like this:

    class MySuperModule : public Module< EntityEvent<RDFEntity>, SPARQLQuery>
    {
        // implement these!
        void process(EntityEvent<RDFEntity>::Ptr) override;
        void process(SPARQLQuery::Ptr) override;
    };
*/
template <class... ObservableTypes> class Module;

template <class ObservableType, class... ObservableTypes>
class Module<ObservableType, ObservableTypes...> : public Module<ObservableTypes...> {
protected:
    Module()
    {
        this->addProcessPair(
            [this](core::Observable::Ptr o) -> bool { return o->isA<ObservableType>(); },
            [this](core::Observable::Ptr o) -> void {
                this->process( core::Observable::as<ObservableType>(o) );
            }
        );
    }

    /**
        An abstract virtual method to handle a specified ObservableType. This has to be implemented
        by the actual module!
    */
    virtual void process(std::shared_ptr<ObservableType>) = 0;

    /**
        The Module-template creates a "overloaded" process method for every specified query type,
        but does so in different classes. Usually the methods would hide each other -- which is in
        this case not a problem because when the query-handle-lambdas are created (ctor) the static
        type of "this" is known and the correct method called (right?). But the compiler issues a
        warning:
        "... hides overloaded virtual function -Woverloaded-virtual". To prevent this we pull all
        handle-methods into the same scope with this using directive:
    */
    using Module<ObservableTypes...>::process;
};

/**
    Anchor: After adding all the ObservableTypes the last step is to derive from
    ModuleBase.
*/
template <>
class Module<> : public ModuleBase {
protected:
    void process(); // unimplemented dummy function to allow using directive in templated class
};


// class Module : public core::Observer{
// private:
//     /*
//         Raw pointer makes things easier in this case. It will be set by the core, and the core
//         will keep a shared_ptr to the module, so the module will die before the core. Everything
//         is fine. :)
//     */
//     core::Core* core_;
//     /* Making core a friend allows it to set the pointer without exposing yet another public method */
//     friend class core::Core;
//
// protected:
//     /**
//         Allows modules to ask queries.
//     */
//     void ask(query::Query::Ptr q);
//
// public:
//     using Ptr = std::shared_ptr<Module>;
//     virtual std::string type() const;
//     virtual ~Module(){}
//
//     /**
//         Called by the core in order to answer a given query.
//         The base-implementation simply calls this->notify(q), allowing
//         Modules to reuse the Observer-Observable-construction: Modules can add
//         supported query-types through addOverload(..) just like in the event
//         system. (Events and Queries both derive from Observable)
//         Besides, Modules are allowed to override this method. The simple
//         notify-call does have its limitations, as you need to register every
//         single Query-type beforhand. E.g. the ActiveObjectStore has its own
//         implementation of "answer".
//
//         Note: I'm not sure if "observer-observable" is the right term here.
//         In lack of better wording ... just stick with it for now.
//     */
//     virtual void answer(query::Query::Ptr q);
// };

} /* processing */

} /* sempr */

#endif /* end of include guard: SEMPR_PROCESSING_MODULE_H_ */
