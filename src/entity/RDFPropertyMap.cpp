#include <sempr/entity/RDFPropertyMap.hpp>
#include <RDFPropertyMap_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(RDFPropertyMap)

RDFPropertyMap::RDFPropertyMap(const core::IDGenBase* idgen,
    const std::string& subject, const std::string& baseURI)
    : RDFEntity(idgen), subject_(subject), baseURI_(baseURI)
{
    setDiscriminator<RDFPropertyMap>();
}

RDFPropertyMap::RDFPropertyMap(const std::string& subject, const std::string& baseURI)
    : RDFPropertyMap(new core::IDGen<RDFPropertyMap>(), subject, baseURI)
{
}


RDFPropertyMap::RDFPropertyMap(const core::IDGenBase* idgen,
    const storage::DBObject& obj, const std::string& baseURI)
    :   RDFEntity(idgen),
        subject_("<" + baseURI + obj.id() + ">"),
        baseURI_(baseURI)
{
    setDiscriminator<RDFPropertyMap>();
}

RDFPropertyMap::RDFPropertyMap(const storage::DBObject& obj, const std::string& baseURI)
    :   RDFPropertyMap(new core::IDGen<RDFPropertyMap>(), obj, baseURI)
{
}

RDFValueProxy RDFPropertyMap::operator[](const std::string& key)
{
    return (*this)(key, baseURI_);
}

RDFValueProxy RDFPropertyMap::operator()(const std::string& key)
{
    return (*this)(key, baseURI_);
}

RDFValueProxy RDFPropertyMap::operator()(const std::string& key, const std::string& baseURI)
{
    // create a unique key to allow the same property under different namespaces.
    // rdf:type, foo:type, bar:type, won't overwrite each other.
    std::string fullKey = baseURI + key;

    auto it = keyValueMap_.find(fullKey);
    if (it == keyValueMap_.end()) {
        // not found, need a new entry.
        RDFPropertyMap::Container entry;
        entry.vectorIndex_ = this->size();

        // a triple of the property map always looks like this:
        // (<subject> <baseURI+?key> ?value propertyMapId)
        Triple t;
        t.subject = subject_;
        t.predicate = "<" + fullKey + ">";
        t.object = ""; // will be set
        this->addTriple(t);

        this->keyValueMap_[fullKey] = entry;
    }

    // return a proxy pointing at the element. It will update the triple
    // when a new value is assigned.
    return RDFValueProxy(this, fullKey);
}


bool RDFPropertyMap::hasProperty(const std::string &key, const std::string &baseURI)
{
    std::string fullKey = baseURI + key;
    return (keyValueMap_.find(fullKey) != keyValueMap_.end());
}

bool RDFPropertyMap::hasProperty(const std::string &key)
{
    return hasProperty(key, baseURI_);
}

void RDFPropertyMap::removeProperty(const std::string &key)
{
    removeProperty(key, baseURI_);
}

void RDFPropertyMap::removeProperty(const std::string &key, const std::string baseURI)
{
    std::string fullKey = baseURI + key;
    auto it = keyValueMap_.find(fullKey);
    if (it == keyValueMap_.end()) return; // nothing there to delete.

    // well. how to remove this entry?
    // problem is: the entries in the map store indices into the actual triple-vector!
    // delete one triple? everything needs to shift!
    // instead, as a workaround before fixing this, just clear the triple (s=p=d="") and remove the
    // entry from the key-value-map, but not from the RDFEntity. That way, there will be a lot of
    // empty triples... but well, workaround.
    this->getTripleAt(it->second.vectorIndex_).subject = "";
    this->getTripleAt(it->second.vectorIndex_).predicate = "";
    this->getTripleAt(it->second.vectorIndex_).object = "";
    keyValueMap_.erase(fullKey);
}

} /* entity */
} /* sempr */
