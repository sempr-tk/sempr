#include <sempr/core/RDF.h>

namespace sempr { namespace core {
    
namespace rdf {
    const std::string& type() {
        static const std::string t = "<www.w3.org/1999/02/22-rdf-syntax-ns#type>";
        return t;
    }
}

namespace rdfs {
#   define RDFS(name) \
    const std::string& (name)() { \
        static const std::string t = "<http://www.w3.org/2000/01/rdf-schema#(name)>"; \
        return t; \
    }
    RDFS(Resource)
    RDFS(Class)
    RDFS(subClassOf)
    RDFS(subPropertyOf)
    RDFS(domain)
    RDFS(range)
#   undef RDFS
}

namespace owl {
#   define OWL(name) \
    const std::string& (name)() { \
        static const std::string& t = "<http://www.w3.org/2002/07/owl#http://www.w3.org/2002/07/owl#(name)>"; \
        return t; \
    }
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


} /* core */
    
} /* sempr */
