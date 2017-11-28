#include <sempr/entity/RDFPropertyMap.hpp>
#include <RDFPropertyMap_odb.h>

namespace sempr { namespace entity {

RDFPropertyMap::RDFPropertyMap(const std::string& subject, const std::string& baseURI)
    : subject_(subject), baseURI_(baseURI)
{
    setDiscriminator<RDFPropertyMap>();
}

RDFPropertyMap::RDFPropertyMap(const storage::DBObject& obj, const std::string& baseURI)
    :   subject_("<" + baseURI + boost::uuids::to_string(obj.uuid()) + ">"),
        baseURI_(baseURI)
{
    setDiscriminator<RDFPropertyMap>();
}

RDFValueProxy RDFPropertyMap::operator[](const std::string& key)
{
    auto it = keyValueMap_.find(key);
    if (it == keyValueMap_.end()) {
        // not found, need a new entry.
        RDFPropertyMap::Container entry;
        entry.vectorIndex_ = this->size();

        // a triple of the property map always looks like this:
        // ()<subject> baseURI+?key> ?value propertyMapId)
        Triple t;
        t.subject = subject_;
        t.predicate = "<" + baseURI_ + key + ">";
        t.object = ""; // will be set
        this->addTriple(t);

        this->keyValueMap_[key] = entry;
    }

    // return a proxy pointing at the element. It will update the triple
    // when a new value is assigned.
    return RDFValueProxy(this, key);
}



} /* entity */
} /* sempr */
