#ifndef SEMPR_COMPONENT_TRIPLEPROPERTYMAP_HPP_
#define SEMPR_COMPONENT_TRIPLEPROPERTYMAP_HPP_

#include <map>
#include <cereal/types/map.hpp>

#include "TripleContainer.hpp"

namespace sempr {

/**
    Helper for the TriplePropertyMap, containing either a simple datatype
    (string, float, int) or a resource (which is a string, just interpreted
    differently).
*/
class TriplePropertyMapEntry {
public:
    enum Type { STRING, FLOAT, INT, RESOURCE, INVALID };
private:
    Type type_;
    std::string strValue_; // also used for resources
    float fltValue_;
    int intValue_;
public:
    /**
        Creates an invalid entry, just to enable map assignments together with
        the (default) assignment operator, for which string, int and float will
        be implicitely cast to TriplePropertyMapEntry using the ctors below.
    */
    TriplePropertyMapEntry();

    TriplePropertyMapEntry(const std::string&, bool isResource = false);
    TriplePropertyMapEntry(const char*, bool isResource = false);
    TriplePropertyMapEntry(float);
    TriplePropertyMapEntry(int);

    TriplePropertyMapEntry& operator = (const TriplePropertyMapEntry& other) = default; // assignment

    operator std::string() const; // cast to string, exception if neither str nor resource
    operator float() const; // cast to float, exception of not float
    operator int() const; // cast to int, exception of not int

    /**
        returns an N3 representation of this entry:
        - floats, ints: value ->  value
        - strings:      value -> "value", " and \ will be escaped to \" and \\
        - resources:    value -> <value>
        NOTE: This does not implement sanity checks. The only modification are
        the surrounding "" or <>, and in case of strings escaping " and \.

        This might not be enough to generate valid N3 from arbitrary values!
    */
    std::string toN3() const;

    /// returns the type of this TriplePropertyMapEntry: STRING, FLOAT, INT, RESOURCE or INVALID
    Type type() const;


    /**
        Serialization with cereal
    */
    template <class Archive>
    void serialize(Archive& ar)
    {
        ar( cereal::make_nvp<Archive>("type", type_),
            cereal::make_nvp<Archive>("strValue", strValue_),
            cereal::make_nvp<Archive>("fltValue", fltValue_),
            cereal::make_nvp<Archive>("intValue", intValue_) );
    }
};



/**
    The TriplePropertyMap is a small wrapper around a std::map with string keys
    and TriplePropertyMapEntry values. These allow us to store strings, floats,
    ints and resources (strings interpreted a bit differently in RDF terms).

    Just use the map_ member to store your values, and call .changed()
    afterwards.

    The resulting triples will have the form:
        subject:    '<' sempr::baseURI() entity->id() '>'
        predicate:  '<' key '>'
        object:     value.toN3()
    So make sure to add e.g. the sempr::baseURI() to your keys in order to make
    the resulting triples valid RDF.
*/
class TriplePropertyMap : public TripleContainer {
    SEMPR_COMPONENT
public:
    using Ptr = std::shared_ptr<TriplePropertyMap>;

    std::map<std::string, TriplePropertyMapEntry> map_;

    TripleIterator begin() const override;
    TripleIterator end() const override;


    /**
        Serialization with cereal
    */
    template <class Archive>
    void save(Archive& ar) const
    {
        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("map", map_) );
    }

    template <class Archive>
    void load(Archive& ar)
    {
        ar( cereal::make_nvp<Archive>("base", cereal::base_class<Component>(this)),
            cereal::make_nvp<Archive>("map", map_) );
    }
};


// register component name
template <>
struct ComponentName<TriplePropertyMap> {
    static constexpr const char* value = "TriplePropertyMap";
};


/**
    Iterator for TriplePropertyMap. Implements the conversion from
    TriplePropertyMapEntry to Triple.
*/
class TriplePropertyMapIterator : public TripleIteratorImpl {
    friend class TriplePropertyMap;
    std::map<std::string, TriplePropertyMapEntry>::const_iterator it_;
    std::string subject_;

    TriplePropertyMapIterator(
            std::map<std::string, TriplePropertyMapEntry>::const_iterator it,
            const std::string& subject
            );

    const Triple operator * () const override; // converts TriplePropertyMapEntry to Triple
    void operator ++ () override;
    bool operator == (const TripleIteratorImpl& other) const override;

public:
    ~TriplePropertyMapIterator();
};

}

CEREAL_REGISTER_TYPE(sempr::TriplePropertyMap)

// also, register to_string function to display TriplePropertyMap instances in
// the rete network
#include <rete-core/Util.hpp>
namespace rete { namespace util {

template <> std::string to_string(const sempr::TriplePropertyMap& c);
template <> std::string to_string(const sempr::TriplePropertyMap::Ptr& c);

}}

#endif /* include guard: SEMPR_COMPONENT_TRIPLEPROPERTYMAP_HPP_ */
