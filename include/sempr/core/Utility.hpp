#ifndef SEMPR_CORE_UTILITY_HPP_
#define SEMPR_CORE_UTILITY_HPP_


#include <type_traits>
#include <odb/core.hxx> // odb::object_traits<T>::base_type

namespace sempr {

namespace entity {
    class Entity;
} /* storage */

namespace core {
    /**
        A helper struct for SFINAE-check if odb::object_traits<T>::base_type exists.
        In c++17: std::void_t
    */
    template <class...> using void_t = void;

    /**
        A helper to return "false" no matter the type given. This is used to let a static_assert
        fail but also get to see the referred type in the error message!
    */
    // template <class...> using false_t = std::false_type;
    template <class...> struct false_t : public std::false_type {};
    // the variant with 'using' does not work correctly: the compiler is able to deduce that it is
    // always false, and complains when creating the precompiled header.


    /**
        type trait to access the base of a class.
        Default is 'void'. Uses ODB type traits whenever possible.
    */
    // template <class T, typename = void>
    // struct base_of {
    //     typedef
    //         typename std::enable_if<not std::is_base_of<storage::DBObject, T>::value>::type
    //         type;
    // };
    template <class T, typename = void>
    struct base_of {
        typedef void type;
    };

    /**
        The specialization for odb::object_traits tries to reference "void" (and crashes.) -- hence
        define a specialization for void here.
    */
    template <>
    struct base_of<void, void> {
        typedef void type;
    };

    /**
        Specialization for everything that has odb::object_traits
    */
    template <class T>
    struct base_of<T, void_t<typename odb::object_traits<T>::base_type>>{
        typedef typename odb::object_traits<T>::base_type type;
    };



    /**
        This helper is used to check if odb::object_traits<T>::base_type exists. If not, and the
        type is a subclass of storage::DBObject/entity::Entity we know that we forgot to include
        the <..._odb.h> header. --> see the next specialization.
    */
    template <class T, class = void> struct has_odb_base : public std::false_type {};
    template <class T>
    struct has_odb_base<T, void_t<typename odb::object_traits<T>::base_type>>
        : public std::true_type
    {
    };

    /**
        Specialization for everything that is derived from DBObject:
            All these classes should be handled by the specialization with odb::object_traits.
            If they are not you forgot to #include <[...]_odb.h>!
    */
    template <class T>
    struct base_of<T,
            typename std::enable_if< std::is_base_of<entity::Entity, T>::value
                                     and not has_odb_base<T>::value
                                   >::type
                  >
    {
        static_assert(false_t<T>::value, "Type is derived from storage::DBObject but does not have a specialization for odb::object_traits. You forgot to #include <[...]_odb.h> !");
    };

} /* core */
} /* sempr */






#endif /* end of include guard: SEMPR_CORE_UTILITY_HPP_ */
