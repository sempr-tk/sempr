#ifndef SEMPR_ENTITY_RDFPROPERTYMAP_HPP_
#define SEMPR_ENTITY_RDFPROPERTYMAP_HPP_

#include <sempr/entity/RDFEntity.hpp>
#include <sempr/entity/RDFValue.hpp>
#include <Soprano/Soprano> // for conversion to rdf-literals.

namespace sempr { namespace entity {

class RDFValueProxy;

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
    RDFValueProxy operator[](const std::string& key);
    RDFValueProxy operator()(const std::string& key);

    /**
        Allows to set a baseURI that differs from the default. To read/write the same variable
        again, you will always need to specify the baseURI, too.
            map["type"] = "foo";
            map["type", "http://example.com/"] = "baz";
            // will _not_ overwrite the previous, except if "http://example.com" is the default
            // baseURI.
    */
    RDFValueProxy operator()(const std::string& key, const std::string& baseURI);

private:
    RDFPropertyMap(){}
    friend class odb::access;
    friend class RDFValueProxy;

    #pragma db value
    struct Container {
        RDFValue value_;        // the actual value
        size_t vectorIndex_;    // the position of the Triple-Entry in RDFEntity
    };

    std::map<std::string, Container> keyValueMap_;
    std::string subject_;
    std::string baseURI_;
};

/** A proxy around a RDFPropertyMap pointing at a fixed element. This is used
    to update the triples of the RDFPropertyMap after altering an RDFValue it is
    pointing at!
*/
class RDFValueProxy {
    // the property map the value resides in. a raw pointer is sufficient, the
    // proxy is NOT intended to be stored - it is just a helper.
    RDFPropertyMap* propertyMap_;
    // defines the entry to point at
    std::string key_;
public:
    RDFValueProxy(RDFPropertyMap* map, const std::string key)
        : propertyMap_(map), key_(key)
    {
    }

    /**
        Assign a value. This updates the RDFValue-Object (which actually
        implments the conversion methods, using Soprano::LiteralValue etc.),
        updates the associated "Triple" and emits a changed-event.
    */
    template <typename T>
    RDFValueProxy& operator = (const T& value) {
        // update the RDFValue
        propertyMap_->keyValueMap_[key_].value_ = value;
        // update the corresponding triple
        size_t index = propertyMap_->keyValueMap_[key_].vectorIndex_;
        Triple& triple = propertyMap_->getTripleAt(index);
        triple.object = propertyMap_->keyValueMap_[key_].value_.toString();
        // emit a changed-event (?)
        // TODO -- maybe not and leave it to the user?
        propertyMap_->changed();
        return *this;
    }

    /** Casts -- implemented by RDFValue */
    template <typename T>
    operator T() const {
        return propertyMap_->keyValueMap_[key_].value_;
    }
};



} /* entity */
} /* sempr */


#endif /* end of include guard: SEMPR_ENTITY_RDFPROPERTYMAP_HPP_ */
