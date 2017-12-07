#ifndef SEMPR_CORE_OBSERVER_H_
#define SEMPR_CORE_OBSERVER_H_

#include <sempr/core/Observable.hpp>

#include <unordered_map>
#include <typeindex>
#include <typeinfo>

namespace sempr { namespace core {

/**
    The Observer-Class allows to register functions for specific types of events
    and queries.
    E.g.:
        AddOverload<Observable>(
            [this](Observable::Ptr foo) { do_something_with(foo); }
        );
    Registers a lambda function that takes an Observable::Ptr and calls
    this->do_something_with(foo) whenever an Observable is to be processed.
    This can be any type of Event or Query.

    Be aware that this only works for specific types: This function will *not*
    be called for any derived event/query, only for specific types.
*/
class Observer {
public:
    using Ptr = std::shared_ptr<Observer>;
    Observer();
    virtual ~Observer();

    /** Called by the EventBroker or for query-handling **/
    void notify(Observable::Ptr);

protected:
    /** registers a new (type-->function) pair **/
    template<typename D, typename F>
    void addOverload(F f) {
        types_[typeid(D)] =
            [f](Observable::Ptr o) {
                f(std::static_pointer_cast<D>(o));
            };
    }

private:
    std::unordered_map<
        std::type_index,
        std::function<void(Observable::Ptr)> > types_;
};

} /* core */

} /* sempr */


#endif /* end of include guard: SEMPR_CORE_OBSERVER_H_ */
