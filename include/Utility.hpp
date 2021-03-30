#ifndef SEMPR_CORE_UTILITY_HPP_
#define SEMPR_CORE_UTILITY_HPP_


#include <type_traits>
#include <functional>
#include <memory>

namespace sempr {
    class Entity;
    class Component;

    // copy the contents of one tuple into a larger one
    // e.g., tuple<int, float> into tuple<int, float, string>
    template <size_t... I, typename T1, typename T2>
    void copy_tuple_impl(T1 const& from, T2& to, std::index_sequence<I...>)
    {
        // The index sequence is just a helper to get a multiple size_t
        // template args in "size_t... I", ranging from 0 to size-1 of the
        // "from" tuple.
        //
        // We need the brace initialization of the dummy-array just to
        // provide a context in which parameter pack expansion is allowed.
        int dummy[] = {
            // The first part is the copy from the source tuple to the
            // destination tuple. The comma-*operator* discards the result
            // of the first expression and always returns the result of the
            // second expression, which in this case is always 0 and
            // provides a valid type for the int-array.
            (std::get<I>(to) = std::get<I>(from), 0)...
            // The ellipsis (...) expands the parameter pack with the given
            // pattern, where the pack (here: I) is replaced by its values:
            // (std::get<0>(to) = std::get<0>(from), 0),
            // (std::get<1>(to) = std::get<1>(from), 0),
            //                [...]
        };
        // the static cast simply suppresses the compiler warning us about
        // the unused variable "dummy".
        static_cast<void>(dummy);
    }

    template <typename T1, typename T2>
    void copy_tuple(T1 const& from, T2& to)
    {
        copy_tuple_impl(from, to,
            std::make_index_sequence<std::tuple_size<T1>::value>());
    }

    /**
        Define a new tuple with more types.
        ExtendTuple<std::tuple<int, float>, float, string, bool>::type
                  = std::tuple<int, float, float, string, bool>
    */
    template <class...> struct ExtendTuple;
    template <class... Ts, class... Us>
    struct ExtendTuple<std::tuple<Ts...>, Us...> {
        typedef std::tuple<Ts..., Us...> type;
    };


    /**
        Return a sub-array from the given array.
    */
    template <size_t Begin, size_t End, class T, size_t N>
    std::array<T, End-Begin> sub_array(const std::array<T, N>& in)
    {
        std::array<T, End-Begin> out;
        std::copy(in.begin() + Begin, in.begin() + End, out.begin());
        return out;
    }

    template <class C>
    struct ComponentName {
        // to be specified by component implementations
        //static constexpr const char* value = "";
    };


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


} /* sempr */






#endif /* end of include guard: SEMPR_CORE_UTILITY_HPP_ */
