#ifndef SEMPR_RDFENTITYI_H_
#define SEMPR_RDFENTITYI_H_

#include <string>
#include <vector>

namespace sempr { namespace core {

namespace rdf {
    const std::string& type();
}

namespace rdfs {
#   define RDFS(name) const std::string& (name)();
    RDFS(Resource)
    RDFS(Class)
    RDFS(subClassOf)
    RDFS(subPropertyOf)
    RDFS(domain)
    RDFS(range)
#   undef RDFS
}

namespace owl {
#   define OWL(name) const std::string& (name)();
    OWL(Class)
    OWL(FunctionalProperty)
    OWL(Nothing)
    OWL(ObjectProperty)
    OWL(Restriction)
    OWL(Thing)
    OWL(allValuesFrom)
    OWL(cardinality)
    OWL(differentFrom)
    OWL(disjointWith)
    OWL(distinctMembers)
    OWL(equivalentClass)
    OWL(equivalentProperty)
    OWL(hasValue)
    OWL(intersectionOf)
    OWL(inverseOf)
    OWL(maxCardinality)
    OWL(minCardinality)
    OWL(onClass)
    OWL(onDataRange)
    OWL(onDatatype)
    OWL(oneOf)
    OWL(onProperty)
    OWL(sameAs)
    OWL(someValuesFrom)
    OWL(unionOf)
#   undef OWL
}

class Triple {

public:
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
};


class RDFEntityI {
public:
    virtual void getTriples(std::vector<Triple>& triples) const = 0;
};

} /* core */
    
} /* sempr */


#endif /* end of include guard: SEMPR_RDFENTITYI_H_ */
