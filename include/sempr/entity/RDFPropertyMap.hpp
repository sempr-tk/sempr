#ifndef SEMPR_ENTITY_RDFPROPERTYMAP_HPP_
#define SEMPR_ENTITY_RDFPROPERTYMAP_HPP_

#include <sempr/entity/RDFVector.hpp>
#include <sempr/entity/RDFValue.hpp>
#include <Soprano/Soprano> // for conversion to rdf-literals.

namespace sempr { namespace entity {

class RDFPropertyMap;

class RDFPropertyMapIterator_impl : public TripleIterator_impl {
    friend class RDFPropertyMap;
    std::map<std::string, RDFValue>::const_iterator it_;
    const RDFPropertyMap* pmap_;

    RDFPropertyMapIterator_impl(const RDFPropertyMap* pmap,
                                std::map<std::string, RDFValue>::const_iterator it);

    const Triple& operator * () const override;
    const Triple* operator -> () const override;
    void operator ++ () override;
    bool operator == (const TripleIterator_impl& other) const override;
};

/**
    The RDFPropertyMap is an extension of the simple RDFEntity. It provides
    convenience methods to easily set and get properties by a shorthand notation
    that are all projected into the triple-format (subject property value).
    "Subject" is a fixed value set in the constructor of the RDFPropertyMap,
    "property" is created from the namespace and the key used to set the property,
    "value" is either a literal-value or a reference to another entity.
*/
#pragma db object
class RDFPropertyMap : public RDFEntity {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<RDFPropertyMap>;
    /** All elements are stored as (subject baseURI+key value) */
    RDFPropertyMap(const std::string& subject, const std::string& baseURI);
    RDFPropertyMap(const core::IDGenBase*, const std::string& subject, const std::string& baseURI);

    /** All elements are stored as (baseURI+obj.id() baseURI+key value) */
    RDFPropertyMap(const storage::DBObject& obj,
                    const std::string& baseURI = sempr::baseURI());
    RDFPropertyMap(const core::IDGenBase*,
                    const storage::DBObject& obj,
                    const std::string& baseURI = sempr::baseURI());

    virtual ~RDFPropertyMap(){}

    TripleIterator begin() const override;
    TripleIterator end() const override;

    /// checks if an entry for a given key exists, without creating one
    bool hasProperty(const std::string& key);
    bool hasProperty(const std::string& key, const std::string& baseURI);

    /// removes a propetry
    void removeProperty(const std::string& key);
    void removeProperty(const std::string& key, const std::string baseURI);

    /**
        Returns a proxy object that can be assigned a new value, or be casted to the type
        that has been stored at the given key. As this accesses an internal map it automatically
        creates a new entry if necessary.
    */
    RDFValue& operator[](const std::string& key);
    RDFValue& operator()(const std::string& key);

    /**
        Allows to set a baseURI that differs from the default. To read/write the same variable
        again, you will always need to specify the baseURI, too.
            map["type"] = "foo";
            map["type", "http://example.com/"] = "baz";
            // will _not_ overwrite the previous, except if "http://example.com" is the default
            // baseURI.
    */
    RDFValue& operator()(const std::string& key, const std::string& baseURI);

private:
    RDFPropertyMap();
    friend class odb::access;
    friend class RDFPropertyMapIterator_impl;

    std::map<std::string, RDFValue> keyValueMap_;
    std::string subject_;
    std::string baseURI_;
};



} /* entity */
} /* sempr */


#endif /* end of include guard: SEMPR_ENTITY_RDFPROPERTYMAP_HPP_ */
