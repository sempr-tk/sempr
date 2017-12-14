#ifndef SEMPR_RDF_H_
#define SEMPR_RDF_H_

#include <string>
#include <vector>

namespace sempr {
    const std::string& baseURI();
    
    namespace core {


namespace rdf {
    const std::string& baseURI();
    const std::string& type();
}

namespace rdfs {
#   define RDFS(name) const std::string& (name)();
    RDFS(baseURI)
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
    OWL(baseURI)
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


#endif /* end of include guard: SEMPR_RDF_H_ */
