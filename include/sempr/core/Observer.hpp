#ifndef SEMPR_CORE_OBSERVER_H_
#define SEMPR_CORE_OBSERVER_H_

#include <sempr/core/Event.hpp>
#include <sempr/core/EntityEvent.hpp>

#include <unordered_map>
#include <typeindex>
#include <typeinfo>

namespace sempr { namespace core {

/**
    The Observer-Class allows to register functions for specific types of events.
    E.g.:
        AddOverload<Event>(
            [this](Event::Ptr foo) { do_something_with(foo); }
        );
    Registers a lambda function that takes an Event::Ptr and calls
    this->do_something_with(foo) whenever such an event is to be processed.
    Be aware that this only works for specific types: This function will *not*
    be called for any derived event, only for specific types.
*/    
class Observer {
public:
    using Ptr = std::shared_ptr<Observer>;
    Observer();
    virtual ~Observer();
    
    /** To be overridden by derived classes **/
    virtual void process(Event::Ptr event);
    
    /** Called by the EventBroker **/
    void notify(Event::Ptr event);
    
protected:
    /** registers a new (type-->function) pair **/
    template<typename D, typename F>
    void addOverload(F f) {
        types_[typeid(D)] = 
            [f](Event::Ptr event) {
                f(std::static_pointer_cast<D>(event));
            };
    }
    
private:
    std::unordered_map<
        std::type_index,
        std::function<void(Event::Ptr)> > types_;
};
    
} /* core */
    
} /* sempr */


#endif /* end of include guard: SEMPR_CORE_OBSERVER_H_ */
