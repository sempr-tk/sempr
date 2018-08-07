#include <sempr/entity/RDFPropertyMap.hpp>
#include <RDFPropertyMap_odb.h>

namespace sempr { namespace entity {

// triple iterator for rdfpropertymap

RDFPropertyMapIterator::RDFPropertyMapIterator(
    const RDFPropertyMap* pmap, std::map<std::string, RDFValue>::const_iterator it)
    : it_(it), pmap_(pmap)
{
}

const Triple RDFPropertyMapIterator::operator*() const
{
    Triple t;
    t.subject = pmap_->subject_;
    t.predicate = "<" + it_->first + ">";
    t.object = it_->second.toString();
    t.document = "<" + sempr::baseURI() + pmap_->id() + ">";

    return t;
}

void RDFPropertyMapIterator::operator++()
{
    ++it_;
}

bool RDFPropertyMapIterator::operator==(const TripleIterator &other) const
{
    auto o = dynamic_cast<const RDFPropertyMapIterator*>(&other);
    if (!o) return false;
    return o->it_ == this->it_;
}

// ----



SEMPR_ENTITY_SOURCE(RDFPropertyMap)

RDFPropertyMap::RDFPropertyMap()
    : RDFEntity(new core::IDGen<RDFPropertyMap>())
{
}


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

RDFValue& RDFPropertyMap::operator[](const std::string& key)
{
    return (*this)(key, baseURI_);
}

RDFValue& RDFPropertyMap::operator()(const std::string& key)
{
    return (*this)(key, baseURI_);
}

RDFValue& RDFPropertyMap::operator()(const std::string& key, const std::string& baseURI)
{
    // create a unique key to allow the same property under different namespaces.
    // rdf:type, foo:type, bar:type, won't overwrite each other.
    std::string fullKey = baseURI + key;
    return keyValueMap_[fullKey];
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

    keyValueMap_.erase(fullKey);
}

TripleIteratorWrapper RDFPropertyMap::begin() const
{
    return TripleIteratorWrapper(new RDFPropertyMapIterator(this, this->keyValueMap_.begin()));
}

TripleIteratorWrapper RDFPropertyMap::end() const
{
    return TripleIteratorWrapper(new RDFPropertyMapIterator(this, this->keyValueMap_.end()));
}

} /* entity */
} /* sempr */
