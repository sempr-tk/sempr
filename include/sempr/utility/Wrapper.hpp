#ifndef SEMPR_CORE_WRAPPER_HPP_
#define SEMPR_CORE_WRAPPER_HPP_

/**
    This file contains a first approach to resolve the problems of the current event-firing-system:
    As it is now, everything inside the database needs to derive from Entity and use the
    ENTITY_DEFAULT_EVENT_METHODS-preprocessor-macro to establish a few methods which make sure that
    whenever Entity::changed() is called, an event is emitted for every single type, e.g.:
        Polygon::changed() calls
            CurvePolygon::changed() calls
                Surface::changed() calls
                    Geometry::changed() calls
                        Entity::changed().

    This is an error-prone system which pollutes the class with additional methods and forces the user
    to derive from a given base class. Since we are talking about a possible re-design of some of
    SEMPRs core-structures to allow for a better event system (real events, track states, changes,
    not a simple pointer) and parallel execution of processing modules, one idea is the following:

    Make Entity *not* a *base class* of everything, but a *wrapper*. We can still use a common
    base class for all entities, but handle the user data structures inside a templated wrapper
    class:
        template <class UserType> class Entity : public BaseEntity { ... };
    Let the BaseEntity / Entity<T> manage common stuff like ids, events and maybe a history of
    the UserType, but keep the UserType as is. (It will still need pragmas for database persistence,
    though).

    NOTE: The code below will not give us a history for entites, or allow parallelism. But it allows
    us to traverse the inheritance-chain of an odb-object (#pragma db object) and inject code for
    every super-class at compile-time. The class called "Wrapper" does exactly that. In its current
    state it could be renamed to "EventEmitter<T>" and used to throw an event for every type in the
    chain. If we implement the Entity<T>-wrapper-class in a similar way this could be promising.
    E.g., extend the Wrapper-template to accept another template-class as a parameter from which
    every layer will inherit, forwarding the current super-type to the given template. That way,
    what is now called "Wrapper" could become a utility-template to traverse the chain and inject
    other template-classes, like an EventEmitter, or a IDGenerator, ...? Maybe this won't work, but
    there is still the alternative of using this pattern to implement more or less everything in the
    Entity<T>-wrapper-class.

    usage:
        Wrapper<sempr::entity::Polygon> w1;
    Will create an instance of a class with the following inheritance-chain:
        Wrapper<Polygon> :
            Wrapper<CurvePolygon> :
                Wrapper<Surface> :
                    ...
                        Wrapper<Entity> :
                            Wrapper<DBObject> :
                                WrapperBase.
    You can exchange WrapperBase by giving it a second parameter:
        Wrapper<sempr::entity::Polygon, Foo> w2foo;
    will have the base-class Foo.
*/

#include <iostream>     // debug output
#include <functional>   // EventEmitter may register methods...
#include <vector>       // ...in a vector

// NOTE:    I've written and commented the code below with the intent of creating an recursive
//          event-emitter. It isn't event complete for that use case, also this pattern could be
//          used for way more than that.


/**
    Below is one possible implementation of the event-emitting-mechanism that creates one event for
    every subtype. It uses CRTP to inject Emitters for every class, which in turn register their
    event-emitting-methods at the entity within their ctor.
    The class-chain is traversed through odbs type traits, which define a base_class
    (and a root_class).
*/

#include <odb/core.hxx>             // odb::object_traits<T>::base_type
// #include <odb/database.hxx>         //
// #include <RDFPropertyMap_odb.h>
// #include <Polygon_odb.h>

//odb::object_traits<sempr::entity::Entity>::base_type
// --> this is the direct parent type. :)
// for Polygon its CurvePolygon, for Person its Entity, ...

/**
    The WrapperBase is used as a recursion-anchor, the absolute base class of all Wrapper<T>-classes.
    It allows to register functions with registerEmit and call them in sequence with one call to
    fireEvents.
*/
// NOTE: registerEmit and fireEvents are currently unused, and also the Wrapper-class accepts any
// class as a base (for which it holds that: base_of<T> == T, see below)
class WrapperBase {
public:
    std::vector<std::function<void()> > callbacks;
    void registerEmit(std::function<void()> f)
    {
        callbacks.push_back(f);
    }

    void fireEvents() {
        for (auto f : callbacks)
        {
            f();
        }
    }
};


/**
    A custom type-trait that defines the direct base-class of the given type. The default-
    implementation simply defines T as a base of T. In the recursive template class defined later
    on we use base_of<T> == T as the anchor.
    If odb::object_traits<T>::base_type exists it is used instead, allowing us to traverse the
    inheritance chain.
*/
template <class T, typename = void>
struct base_of {
    typedef T type;
};

/**
    A helper struct for SFINAE-check of odb::object_traits<T>::base_type exists.
    1.  If it exists, void_<T>::type is defined as void, and hence the template specialization for
            template <class T> struct base_of<T, void>
        is used instead of the general
            template <class T, typename = void> struct base_of,
        thus giving us odb::object_traits<T>::base_type as base_of T.
    2.  If it does not exist, the 'T' in void_<T> is invalid, and the substitution for void_<T>::type
        fails. Therefore, the specialized template definition is discarded (silently --> SFINAE),
        and the general case is used (which defines T as base_of T).
*/
template <typename> struct void_ { typedef void type; };

/**
    Template specialization of base_of to use odb::object_traits<T>::base_type whenever possible.
*/
template <class T>
struct base_of<T, typename void_<typename odb::object_traits<T>::base_type>::type> {
    typedef typename odb::object_traits<T>::base_type type;
};


/**
    Recursion anchor: When trying to instanciate Wrapper<T, Base> with Base = T we are already
    beyond the root class in the chain and have reached the the base which shall be used for the
    wrapper.
*/
template <class T, class WBase, class TBase> class Wrapper;

template <class T, class WBase>
class Wrapper<T, WBase, T> : public WBase {};

/**
    This is where the actual template magic happens.
    Wrapper<T> inherits Wrapper<Base>, where Base depends on T:
        if T == base_of<T>, then Base = WrapperBase
        if T != base_of<T>, then Base = base_of<T>.

    That way, we create the inheritance graph:
    Wrapper<GrandChild, Child> :
        Wrapper <Child, Parent> :
            Wrapper<Parent, WrapperBase> :
                Wrapper<WrapperBase, WrapperBase> :
                    WrapperBase;

    This way, when creating Wrapper<Foo> we can do a lot of cool stuff with all the types of Foo,
    though not with the Foo-instance itself.

    The goal of this implementation was to enable an easy setup of event-emitters: "Wrapper<T>" could
    be "EventEmitter<T>", which registers a method that can be triggered through WrapperBase.
*/
template <  class T,
            class WBase = WrapperBase,
            class TBase =   typename std::conditional<
                                not std::is_same<T, typename base_of<T>::type>::value,
                                typename base_of<T>::type,
                                WBase
                            >::type >
class Wrapper : public Wrapper<TBase, WBase>
{
public:
    Wrapper() {
        std::cout << "Ctor of wrapper for type: " << typeid(T).name() << '\n';
    }
};



/*** END OF GENERIC CODE ***/
/*** START OF DEBUG STUFF ***/

class Foo {
public:
    Foo() { std::cout << "Ctor of Foo." << '\n'; }
};


// int main(int, char**) {
//     std::cout << "--------- Polygon, WrapperBase ----------" << '\n';
//     Wrapper<sempr::entity::Polygon, WrapperBase> w1;
//
//     std::cout << "--------- Polygon, Foo as Base ----------" << '\n';
//     Wrapper<sempr::entity::Polygon, Foo> w1foo;
//
//     std::cout << "------ RDFPropertyMap, WrapperBase ------" << '\n';
//     Wrapper<sempr::entity::RDFPropertyMap, WrapperBase> w2;
//
//     std::cout << "------ RDFPropertyMap, Foo as Base ------" << '\n';
//     Wrapper<sempr::entity::RDFPropertyMap, Foo> w2foo;
//
//
//     return 0;
// }





#endif /* end of include guard SEMPR_CORE_WRAPPER_HPP_ */
