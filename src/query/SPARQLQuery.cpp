#include <sempr/query/SPARQLQuery.hpp>
#include <sempr/core/RDF.hpp>

namespace sempr { namespace query {

SPARQLQuery::SPARQLQuery()
{
    // set predefined prefixes for sparql queries.
    using namespace sempr::core;
    prefixes["rdf"] = rdf::baseURI();
    prefixes["rdfs"] = rdfs::baseURI();
    prefixes["owl"] = owl::baseURI();
    prefixes["sempr"] = sempr::baseURI();
}

std::string SPARQLQuery::toString() const
{
    std::string q = "";
    for (auto p : this->prefixes)
    {
        q += "@prefix " + p.first + ": <" + p.second + "> . \n";
    }
    q += this->query;
    return q;
}


}}
