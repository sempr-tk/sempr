#ifndef SEMPR_CORE_UTILITY_HPP_
#define SEMPR_CORE_UTILITY_HPP_


#include <odb/core.hxx> // odb::object_traits<T>::base_type

namespace sempr { namespace core {
    /**
        A helper struct for SFINAE-check if odb::object_traits<T>::base_type exists.
        In c++17: std::void_t
    */
    template <class...> using void_t = void;


    /**
        type trait to access the base of a class.
        Default is 'void'. Uses ODB type traits whenever possible.
    */
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
        Specialization for odb::object_traits
    */
    template <class T>
    struct base_of<T, void_t<typename odb::object_traits<T>::base_type>>{
        typedef typename odb::object_traits<T>::base_type type;
    };
} /* core */
} /* sempr */






#endif /* end of include guard: SEMPR_CORE_UTILITY_HPP_ */
