#ifndef SEMPR_UTIL_COMPONENTDEFS_HPP_
#define SEMPR_UTIL_COMPONENTDEFS_HPP_

#include <type_traits>

namespace cereal {
    class JSONInputArchive;
}

namespace sempr {

/**
    This structure checks if the given template argument type has a
        void load<cereal::JSONInputArchive>(cereal::JSONInputArchive&)
    method. Every component in sempr is required to implement this -- better as
    two templated load and save methods to support all kinds of cereal archives.
    But I couldn't figure out how to add the template argument in this check,
    and only need the JSONInputArchive version for the implementation of the
        virtual void loadFromJSON(cereal::JSONInputArchive&)
    method.
*/
template <class T> // T is the type we want to check for the method
struct has_load_method {
private:
    // the check will work by comparing the return type of a meta-function
    typedef std::true_type yes;
    typedef std::false_type no;

    // this is a helper struct that is only defined if the second template
    // argument has the signature we want: A member function of the first
    // template argument (U::*), that returns void and takes a reference to a
    // cereal::JSONInputArchive as an argument:
    template <class U, void (U::* f)(cereal::JSONInputArchive&)> struct SFINAE {};
    // (SFINAE: Substitution Failure Is Not An Error)

    // this is the meta-function that tests if a given class C has a method with
    // the name we want, namely load<cereal::JSONInputArchive>. The more
    // specific version takes a pointer to the SFINAE struct above. But this is
    // only valid if the template arguments match the specification in the
    // declaration of SFINAE -- this is the check on the signature.
    template <class C> static yes test(SFINAE<C, &C::template load<cereal::JSONInputArchive>>*);
    // where this SFINAE thingy results in a substitution failure, i.e. the
    // class C does not have this load method with the signature as required by
    // the SFINAE struct, this alternative is okay with anything, but its
    // return type is "no" instead:
    template <class C> static no test(...);
public:
    // and here we apply the test to the actually given class T:
    // if the type that test<T>(nullptr) would return is yes, the substitution
    // was successfull and T has a load method with the matching signature.
    static constexpr bool value = std::is_same<yes, decltype(test<T>(nullptr))>::value;
};


/**
    The SEMPR_COMPONENT macro checks if the class implements the required
    deserialization method, and uses it to implement the loadFromJSON method
    which allows updating a component through a base pointer from a json
    representation.
*/
#define SEMPR_COMPONENT \
    void loadFromJSON(cereal::JSONInputArchive& ar) override \
    { \
        static_assert(has_load_method<std::decay<decltype(*this)>::type>::value, \
                "Error: SEMPR_COMPONENT is missing the templated load method for " \
                "deserialization with cereal."); \
        ar(*this); \
    }

}

#endif /* include guard: SEMPR_UTIL_COMPONENTDEFS_HPP_ */
