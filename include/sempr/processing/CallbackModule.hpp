#ifndef SEMPR_PROCESSING_CALLBACK_MODULE_HPP_
#define SEMPR_PROCESSING_CALLBACK_MODULE_HPP_


#include "Module.hpp"
#include "../core/Observable.hpp"
#include "../core/Utility.hpp"
#include <functional>

namespace sempr {
namespace processing {


/**
    The CallbackModule is a convenience module to easily allow adding simple callback functions
    to react to changes on one specific type of entity.
    Instantiate it with a callback, e.g. a lambda function, which takes a shared_ptr to an
    ObservableType as input, e.g. an EntityEvent<...>.
*/
template <class ObservableType>
class CallbackModule : public Module<ObservableType> {
public:
    typedef std::function<void(std::shared_ptr<ObservableType>)> observable_cb;
    observable_cb callback_;
public:
    using Ptr = std::shared_ptr<CallbackModule>;

    CallbackModule(observable_cb callback)
        : callback_(callback)
    {
    }

protected:
    void process(std::shared_ptr<ObservableType> observable) override
    {
        if (callback_) callback_(observable);
    }
};

// convenience helper to deduce template parameters
template <class Func, // functor/function/member function/lambda given
          class FuncType = typename core::function_traits<Func>::f_type, // matching std::function
          class ObservableType = typename core::remove_shared_ptr<       // derive observable-type 
                                    typename core::extract_type<         // from the first argument
                                        typename core::function_traits<Func>::arg_types
                                    , 0>::type
                                 >::type>
typename CallbackModule<ObservableType>::Ptr CreateCallbackModule(Func f)
{
    FuncType cb = f;
    return typename CallbackModule<ObservableType>::Ptr(new CallbackModule<ObservableType>(cb));
}

}
}

#endif
