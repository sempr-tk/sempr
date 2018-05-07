#ifndef SEMPR_CORE_OBSERVER_H_
#define SEMPR_CORE_OBSERVER_H_

#include <sempr/core/Observable.hpp>

#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <functional>
#include <vector>

namespace sempr { namespace core {

/**
    The Observer-Class allows to register functions-pairs that operate on Observables. The first
    function checks if the Observable (Event or Query) shall/can be processed, and the second
    actually does the processing.
    The function pairs are encapsuled in a protected struct Observer::ProcessPair. It's main use
    is in the Module class, which abstracts it away using lambdas and providing virtual methods to
    be overridden by the user.
*/
class Observer {
protected:
    struct ProcessPair {
        typedef std::function<bool(Observable::Ptr)> Check_f;
        typedef std::function<void(Observable::Ptr)> Process_f;

        Check_f checkFunction;
        Process_f processFunction;
    };

    /// The list of registered ProcessPairs.
    std::vector<ProcessPair> processPairs_;


public:
    using Ptr = std::shared_ptr<Observer>;
    Observer();
    virtual ~Observer();

    /**
        Called by the EventBroker or for query-handling

        This method simply iterates over all registered ProcessPairs and calls all process-methods
        for which the check-function returned 'true'.
        TODO: really call *all* of them? Or do we want to start with the last added pair and only call the one with the first successfull check? That way, when a module defines methods to process Geometry *and* Polygon, only the Polygon-one would be called. Well, no need to decide now, this can be added with a flag later on.
    */
    void notify(Observable::Ptr);

    /**
        Check if at least one of the ProcessingPairs can process the given Observable.
    */
    bool canProcess(Observable::Ptr);

    /**
        Alias to void notifiy(Observable::Ptr)
    */
    void doProcess(Observable::Ptr);


protected:
    /** registers a new (type-->function) pair **/
    // template<typename D, typename F>
    // void addOverload(F f) {
    //     types_[typeid(D)] =
    //         [f](Observable::Ptr o) {
    //             f(std::static_pointer_cast<D>(o));
    //         };
    // }
    void addProcessPair(ProcessPair p);
    void addProcessPair(ProcessPair::Check_f check, ProcessPair::Process_f process);



private:
    // std::unordered_map<
    //     std::type_index,
    //     std::function<void(Observable::Ptr)> > types_;

};

} /* core */

} /* sempr */


#endif /* end of include guard: SEMPR_CORE_OBSERVER_H_ */
