#ifndef SEMPR_CORE_OBSERVABLE_HPP_
#define SEMPR_CORE_OBSERVABLE_HPP_

#include <memory>

#include <set>
#include <typeinfo>
#include <typeindex>
#include <iostream>

namespace sempr { namespace core {

/**
    This class defines a basic "Observable". It is the base for all events and queries, as they are
    similar in structure. It provides an interface to allow subclasses to register their types in
    this base class, so that event with only an Observable* you can ask (and cast) it to one of the
    registered sub-types.
    NOTE: All in all it is just a way to avoid dynamic_casts by saving type_index in a set and using
    static_cast if the type was found. Hopefully this is faster (since less powerfull than
    dynamic_cast), and at least a bit more readable. Syntactic sugar.
*/
class Observable {
    /** Stores the concrete types of this object. Things we can safely static_cast this to. */
    std::set<std::type_index> types_;
    /** Friend declaration for more syntactic sugar. See the class definition below. */
    template <class T> friend class OType;

protected:
    /** Allows subclasses to register their type */
    template <class T>
    void registerType() { types_.insert(typeid(T)); }

public:
    using Ptr = std::shared_ptr<Observable>;
    virtual ~Observable() {}

    /** Check if this can safely be static_cast to T */
    template <class T>
    bool isA() const { return types_.find(typeid(T)) != types_.end(); }

    /** Static cast to T */
    template <class T>
    T& as() { return static_cast<T>(*this); }

    // TODO: Is there a way to define this cast for shared_ptr, too? Maybe like this: Usage: SomethingElse::Ptr something = Observable::as<SomethingElse>(observable);
    template <class T>
    static std::shared_ptr<T> as(Observable::Ptr observable)
    {
        return std::static_pointer_cast<T>(observable);
    }

    /// print all registered classes of an Observable
    friend std::ostream& operator << (std::ostream& os, const Observable& observable);
};

// NOTE: or like this, maybe? Usage: ... = sempr::core::as<SomethingElse>(observable);
template <class T>
std::shared_ptr<T> as(Observable::Ptr observable)
{
    // QUESTION: Check observable->isA<T>() and return nullptr if not? Or assume the user does this?
    return std::static_pointer_cast<T>(observable);
}


/// print all registered classes of an Observable
std::ostream& operator << (std::ostream& os, const Observable& observable);


/**
    TODO: Better name than "OType"? Just "Type" (its in a namespace, so why not?)?
    CRTP class for a nicely looking injection of type-information.
    Usage:
        class MyClass : public Observable, public OType<MyClass> {}

    If mostly identical to:

        class MyClass : public Observable {
        public:
            MyClass() {
                registerType<MyClass>();
            }
        }

    And allows:
        Observable* o = new MyClass();
        o->isA<MyClass>(); // returns true
*/
template <class T>
class OType {
protected:
    OType() {
        static_assert(std::is_base_of<OType<T>, T>::value,
            "Invalid use of CRTP class: OType<T> must be base of T.");

        // this requires that Observable is a base of ~>T<~, but actually we only need
        // to know that Observable is a base of ~>this<~!
        // static_assert(std::is_base_of<Observable, T>::value,
        //     "OType<T> can only be used with a sempr::core::Observable.");

        // so try again with decltype
        static_assert(std::is_base_of<Observable, T>::value,
            "OType<T> can only be used with a sempr::core::Observable.");

        // this is the magic: Register the type T at the Observable
        static_cast<T*>(this)->template registerType<T>();
    }
};



}}

#endif /* end of include guard SEMPR_CORE_OBSERVABLE_HPP_ */
