// TODO: actually, this is not an "entity", but a value. (object vs value in odb)
#ifndef SEMPR_ENTITY_TRIPLE
#define SEMPR_ENTITY_TRIPLE

namespace sempr { namespace entity {

#pragma db value
struct Triple {
    Triple() {}
    
    Triple(const std::string& s, const std::string& p, const std::string& o)
        : subject(s), predicate(p), object(o), document("")
    {
    }

    Triple(const std::string& s, const std::string& p, const std::string& o,
            const std::string& d)
        : subject(s), predicate(p), object(o), document(d)
    {
    }

    Triple(const Triple& other)
        : subject(other.subject), predicate(other.predicate),
            object(other.object), document(other.document)
    {
    }

    std::string subject, predicate, object, document;

    bool operator==(const Triple& other)
    {
        return (subject == other.subject) &&
                (predicate == other.predicate) &&
                (object == other.object) &&
                (document == other.document);
    }
};

} /* entity */
} /* sempr */

#endif /* end of include guard: SEMPR_ENTITY_TRIPLE */
