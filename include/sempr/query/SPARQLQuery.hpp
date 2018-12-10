#ifndef SEMPR_QUERY_SPARQLQUERY_HPP_
#define SEMPR_QUERY_SPARQLQUERY_HPP_

#include <sempr/query/Query.hpp>
#include <map>
#include <vector>

namespace sempr { namespace query {

    class SPARQLQuery : public Query, public core::OType<SPARQLQuery> {
    public:
        using Ptr = std::shared_ptr<SPARQLQuery>;
        SPARQLQuery();

        std::string type() const override { return "SPARQLQuery"; }

        /**
            Returns the full query-string, including prefixes.
        */
        std::string toString() const;


        /**
            Maps prefixes to resource URIs. Allows to use "rdf:type" instead of
            "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>" by specifying
            the mapping
            prefixes["rdf"] = "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>"

            Predefined by default are: rdf, rdfs, owl, sempr
        */
        std::map<std::string, std::string> prefixes;

        /**
            The query-string. Something like
                SELECT ?s
                WHERE {
                    ?s ?p ?o .
                }
        */
        std::string query;


        enum ValueType {
            RESOURCE = 0,
            LITERAL,
            BLANK
        };
        typedef std::pair<ValueType, std::string> Value;
        typedef std::map<std::string, Value> Bindings;

        /**
            Stores the results generated by processing modules.
            Every entry in the vector is one result, containing the bindings
            of the variables in the map.
        */
        std::vector<Bindings> results;
    };

}}

#endif /* end of include guard SEMPR_QUERY_SPARQLQUERY_HPP_ */
