#ifndef SEMPR_PROCESSING_SOPRANOMODULE_HPP_
#define SEMPR_PROCESSING_SOPRANOMODULE_HPP_

#include <sempr/processing/Module.hpp>
#include <sempr/entity/RDFEntity.hpp>
#include <sempr/entity/RuleSet.hpp>
#include <sempr/query/SPARQLQuery.hpp>
#include <Soprano/Soprano>
#include <Soprano/Inference/InferenceModel>


namespace sempr { namespace processing {
    /**
        Provides some basic reasoning- and query-capabilities: Listens for
        "RDFEntity"s and adds them to a soprano-model in order to answer
        SPARQL-queries.
    */
    class SopranoModule : public Module {
    public:
        using Ptr = std::shared_ptr<SopranoModule>;
        SopranoModule();
        ~SopranoModule();

        std::string type() const { return "SopranoModule"; }

        /**
            add / remove triples belonging to the event to the soprano module
        */
        void process(core::EntityEvent<entity::RDFEntity>::Ptr event);

        /**
            add / remove rules
        */
        void process(core::EntityEvent<entity::RuleSet>::Ptr event);

        /**
            answer a SPARQLQuery
        */
        void answer(query::SPARQLQuery::Ptr query);

    private:
        /// all updates take place inside the base model
        Soprano::Model* model_;
        /// sparql queries are performed on the inference model
        Soprano::Inference::InferenceModel* infmodel_;
        /// if model and infmodel are out of sync, this flag is set and the next sparqlquery
        /// triggers a performInference
        bool dirty_;

        /// a mapping between RuleSet-entities (string id) and their rules
        std::map<std::string, std::vector<Soprano::Inference::Rule> > ruleMap_;

    };
}}

#endif /* end of include guard SEMPR_PROCESSING_SOPRANOMODULE_HPP_ */
