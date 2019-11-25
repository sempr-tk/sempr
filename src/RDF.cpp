#include "RDF.hpp"

namespace sempr {
    const std::string& baseURI() {
        /**
            An entity inside the database is not available through some silly "http://whatever.com/sempr/EntityId",
            hence we just reduce the base uri to "sempr:" too keep it short and simple.

            Note: Not "sempr://"! The specification of URIs says that those two slashes are present
            iff the authority part of the URI (scheme://authority/path/to/somewhere?query=sth#fragment)
            is needed. This would make "Person_7" in "sempr://Person_7" an authority (like google.com)
            instead of a path. A practical problem: QUrl converts authority-names to lowercase, so
            every custom class or id or property would be changed.
        */
        static const std::string t = "sempr:";
        return t;
    }

    namespace core {

namespace xsd {
    const std::string& baseURI() {
        static const std::string t = "http://www.w3.org/2001/XMLSchema#";
        return t;
    }
}

namespace rdf {
    const std::string& baseURI() {
        static const std::string t = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
        return t;
    }

    const std::string& type() {
        static const std::string t = "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>";
        return t;
    }
}

namespace rdfs {
    const std::string& baseURI() {
        static const std::string t = "http://www.w3.org/2000/01/rdf-schema#";
        return t;
    }

#   define RDFS(name) \
    const std::string& (name)() { \
        static const std::string t = "<http://www.w3.org/2000/01/rdf-schema#" #name ">"; \
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
    const std::string& baseURI() {
        static const std::string& t = "http://www.w3.org/2002/07/owl#";
        return t;
    }
#   define OWL(name) \
    const std::string& (name)() { \
        static const std::string& t = "<http://www.w3.org/2002/07/owl#" #name ">"; \
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
