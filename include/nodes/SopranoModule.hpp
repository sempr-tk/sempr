#ifndef SEMPR_SOPRANOMODULE_HPP_
#define SEMPR_SOPRANOMODULE_HPP_

#include <rete-core/Production.hpp>
#include <rete-core/Accessors.hpp>
#include <rete-rdf/Triple.hpp>

#include <Soprano/Soprano>

namespace sempr {

/**
    A helper class containing everything around a single sparql query --
    the query itself, prefix definitions, and the results.
*/
class SPARQLQuery {
public:
    SPARQLQuery();

    /**
        Returns the full query string, including prefix definitions
    */
    std::string toString() const;

    /**
        Maps prefixes to resource URIs. Allows to write e.g. "rdf:type"
        instead of "<http://www.w3.org/1999/02/22-rdf-syntax-ns#type>".

        Predefined by default are: rdf, rdfs, owl, sempr
    */
    std::map<std::string, std::string> prefixes;

    /**
        The query string. Note that the SopranoModule only allows read-only
        queries, like
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
        Stores the results.
        Every entry in the vector is one result, containing the bindings of
        the variable in the map.
    */
    std::vector<Bindings> results;
};


/**
    The SopranoModule has only one purpose: To collect all triples and to answer
    SPARQL queries on demand.

    To properly integrate this into the reasoner, the functionality is split
    into multiple parts:
    - The SopranoModule does nothing else than answering queries, and provides
      an interfaceto add/retract triples.
    - The SopranoNode gets a pointer to the SopranoModule and updates it
      with the triples it gets through the rule it is integrated in.
    - The SopranoNodeBuilder connects all SopranoNodes to the same instance of
      SopranoModule.
    This way there can be multiple rules that add triples to the SopranoModule,
    though in 99.9% this will not be necessary, and simply all triples are added
    through [(?s ?p ?o) -> SopranoModule(?s ?p ?o)]. But this also allows us to
    get access to the SopranoModule with its query answering capability -- if it
    was contained in the SopranoNode this would be difficult to aquire.
*/
class SopranoModule {
    // TODO: So far we have no "TripleNode" datatype as we have strings, numbers and components!
    // TripleAccessor is merely an implementation of StringAccessor, and thus returns the contents of
    // triples as strings... In this case we want to have them as actual nodes, as
    // <http://some.random/resource#foo> or "42"^^<xsd:Integer> etc...
    // Whereas with a StringAccessor we expect "42" (without quotation marks),
    // and maybe "http://....#foo" (without <>, without "")?
    Soprano::Model* model_;

    static Soprano::Statement StatementFromSPO(const std::string& subject,
                                               const std::string& predicate,
                                               const std::string& object);

public:
    using Ptr = std::shared_ptr<SopranoModule>;

    SopranoModule();
    ~SopranoModule();

    /**
        Answer the SPARQLQuery. Writes the results into the queries result vector
    */
    void answer(SPARQLQuery& query) const;

    void addTriple(const std::string& subject,
                   const std::string& predicate,
                   const std::string& object);

    void removeTriple(const std::string& subject,
                      const std::string& predicate,
                      const std::string& object);
};

/**
    The SopranoNode class is responsible for updating the SopranoModule
*/
class SopranoNode : public rete::Production {
    SopranoModule::Ptr module_;
    rete::PersistentInterpretation<std::string> sub_, pred_, obj_;

public:
    using Ptr = std::shared_ptr<SopranoNode>;
    SopranoNode(SopranoModule::Ptr,
            rete::PersistentInterpretation<std::string> sub,
            rete::PersistentInterpretation<std::string> pred,
            rete::PersistentInterpretation<std::string> obj);

    /**
        Update the Soprano::Model. Does not infer anything.
    */
    void execute(rete::Token::Ptr, rete::PropagationFlag, std::vector<rete::WME::Ptr>&) override;

    /**
        For visualization
    */
    std::string toString() const override;

};

}


#endif /* include guard: SEMPR_SOPRANOMODULE_HPP_ */
