#ifndef SEMPR_PROCESSING_SOPRANOMODULE_HPP_
#define SEMPR_PROCESSING_SOPRANOMODULE_HPP_

#include <RDFEntity_odb.h>
#include <RuleSet_odb.h>

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
    class SopranoModule
        : public Module< core::EntityEvent<entity::RDFEntity>,
                         core::EntityEvent<entity::RuleSet>,
                         query::SPARQLQuery > {
    public:
        using Ptr = std::shared_ptr<SopranoModule>;
        SopranoModule(bool reasoning = true);
        ~SopranoModule();

        std::string type() const override { return "SopranoModule"; }

        /**
            add / remove triples belonging to the event to the soprano module
        */
        void process(core::EntityEvent<entity::RDFEntity>::Ptr event) override;

        /**
            add / remove rules
        */
        void process(core::EntityEvent<entity::RuleSet>::Ptr event) override;

        /**
            answer a SPARQLQuery
        */
        void process(query::SPARQLQuery::Ptr query) override;

        /**
            Get true if soprano is used for reasoning. Default is "true".
        */
        bool reasoningEnabled();

    private:
        /// all updates take place inside the base model
        Soprano::Model* model_;
        /// sparql queries are performed on the inference model
        Soprano::Inference::InferenceModel* infmodel_;
        /// if model and infmodel are out of sync, this flag is set and the next sparqlquery
        /// triggers a performInference
        bool dirty_;

        const bool reasoningEnabled_;

        /// a mapping between RuleSet-entities (string id) and their rules
        std::map<std::string, std::vector<Soprano::Inference::Rule> > ruleMap_;

    };
}}

#endif /* end of include guard SEMPR_PROCESSING_SOPRANOMODULE_HPP_ */
