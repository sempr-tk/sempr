#ifndef SEMPR_CORE_UTILITY_HPP_
#define SEMPR_CORE_UTILITY_HPP_


#include <type_traits>
#include <odb/core.hxx> // odb::object_traits<T>::base_type
#include <functional>
#include <memory>

namespace sempr {

namespace entity {
    class Entity;
} /* storage */

namespace core {

    template <typename T>
    struct remove_shared_ptr {
        typedef T type;
    };

    template <typename T>
    struct remove_shared_ptr<std::shared_ptr<T> > {
        typedef T type;
    };

    template <typename T>
    struct is_shared_ptr {
        static constexpr bool value = false;
    };

    template <typename T>
    struct is_shared_ptr<std::shared_ptr<T> > {
        static constexpr bool value = true;
    };


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
        Helper to get the odb discriminator for a class
    */
    template <class T>
    struct odb_discriminator
    {
        static constexpr const std::string& value =
            odb::object_traits_impl<T, odb::id_common>::info.discriminator;
    };

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


    /**
        Working with tuples: Extract the type of the I'th entry in the tuple
    */
    template <size_t I, typename T1, typename... Ts>
    struct index_type_pack : public index_type_pack<I-1, Ts...> {};

    template <typename T1, typename... Ts>
    struct index_type_pack<0, T1, Ts...> {
        typedef T1 type;
    };

    template <typename T, size_t = 0>
    struct extract_type {
        typedef T type;
    };

    template <template <typename...> class V, size_t I, typename... Ts>
    struct extract_type<V<Ts...>, I> : index_type_pack<I, Ts...> {};


    /**
        Function traits: Get the signature of a function, method, functor, .. as a std::function<...>
        https://stackoverflow.com/a/21665705
    */

    // functors: delegate to 'operator()'
    template <typename T>
    struct function_traits
        : public function_traits<decltype(&T::operator())>
    {};

    // pointer to member function
    template <typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType(ClassType::*)(Args...) const> {
        typedef std::function<ReturnType(Args...)> f_type;
        typedef ReturnType return_type;
        typedef std::tuple<Args...> arg_types;
    };

    // function pointers
    template <typename ReturnType, typename... Args>
    struct function_traits<ReturnType (*)(Args...)> {
        typedef std::function<ReturnType(Args...)> f_type;
        typedef ReturnType return_type;
        typedef std::tuple<Args...> arg_types;
    };



} /* core */
} /* sempr */






#endif /* end of include guard: SEMPR_CORE_UTILITY_HPP_ */
