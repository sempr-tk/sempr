// TODO: actually, this is not an "entity", but a value. (object vs value in odb)
#ifndef SEMPR_ENTITY_TRIPLE
#define SEMPR_ENTITY_TRIPLE

#include <string>
#include <sempr/entity/RDFValue.hpp>

namespace sempr { namespace entity {

#pragma db value
struct Triple {         //deprecated could be replaced by the RDFValueTriple as class with an subject(), predicate(), object() getter for the strings. ToDo!
    Triple() {}

    Triple(const RDFValueTriple& valueTriple)   // allow implicit type convert from complex to simple triple
    {
        subject = valueTriple.subject.toString();
        predicate = valueTriple.predicate;
        object = valueTriple.object.toString();
    }

    Triple(const std::string& s, const std::string& p, const std::string& o)
        : subject(s), predicate(p), object(o)
    {
    }

    Triple(const Triple& other)
        : subject(other.subject), predicate(other.predicate),
            object(other.object)
    {
    }

    std::string subject, predicate, object;

    bool operator==(const Triple& other)
    {
        return  (subject == other.subject) &&
                (predicate == other.predicate) &&
                (object == other.object);
    }
};

} /* entity */
} /* sempr */

#endif /* end of include guard: SEMPR_ENTITY_TRIPLE */
