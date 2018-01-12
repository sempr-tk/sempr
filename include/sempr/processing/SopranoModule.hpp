#ifndef SEMPR_PROCESSING_SOPRANOMODULE_HPP_
#define SEMPR_PROCESSING_SOPRANOMODULE_HPP_

#include <sempr/processing/Module.hpp>
#include <sempr/entity/RDFEntity.hpp>
#include <sempr/query/SPARQLQuery.hpp>
#include <Soprano/Soprano>


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
            answer a SPARQLQuery
        */
        void answer(query::SPARQLQuery::Ptr query);

    private:
        Soprano::Model* model_;
    };
}}

#endif /* end of include guard SEMPR_PROCESSING_SOPRANOMODULE_HPP_ */
