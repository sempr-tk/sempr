#include <sempr/sempr.hpp>
using namespace sempr::storage;

#include <sempr/entity/CoffeeMug.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
using namespace sempr::entity;

#include <sempr/processing/DebugModule.hpp>
#include <sempr/processing/DBUpdateModule.hpp>
#include <sempr/processing/ActiveObjectStore.hpp>
#include <sempr/processing/SopranoModule.hpp>
using namespace sempr::processing;

#include <sempr/query/ObjectQuery.hpp>
using namespace sempr::query;

#include <fstream>
#include <iostream>

#include <odb/database.hxx>
#include <Person_odb.h>

#include <sempr/entity/spatial/Point.hpp>
#include <Point_odb.h>

#include <sempr/entity/spatial/GeometryCollection.hpp>
#include <RuleSet_odb.h>

#include <sempr/core/IncrementalIDGeneration.hpp>

#include <cpl_conv.h> // CPLFree for kml export of geometries

#include <sempr/entity/spatial/LocalCS.hpp>

#ifndef M_PI
#   define M_PI 3.141592653589793
#endif

void print(OGRGeometry* p)
{
    char* str;
    p->exportToWkt(&str, wkbVariantIso);
    std::cout << str << '\n';
    CPLFree(str);
}


// create a simple query
class DummyQuery : public Query {
public:
    using Ptr = std::shared_ptr<DummyQuery>;
    std::string type() const { return "DummyQuery"; }
    int count;
};

// create a simple module that will just issue a sparql-query.
class DummyModule : public Module {
public:
    using Ptr = std::shared_ptr<DummyModule>;
    std::string type() const {
        return "DummyModule";
    }

    DummyModule() {
        addOverload<DummyQuery>(
            [this] (DummyQuery::Ptr q) {
                this->answerDummy(q);
            }
        );
    }

    void answerDummy(DummyQuery::Ptr query)
    {
        // just ask a sparql-query
        SPARQLQuery::Ptr sparql(new SPARQLQuery());
        sparql->query = "SELECT * WHERE { ?s ?p ?o . }";
        this->ask(sparql);
        // result of DummyQuery is the number of results of the sparql query
        query->count = sparql->results.size();
    }
};

int main(int argc, char** args)
{
    // ODBStorage::Ptr storage( new ODBStorage(":memory:") );
    ODBStorage::Ptr storage( new ODBStorage() );

    DebugModule::Ptr debug( new DebugModule() );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );
    SopranoModule::Ptr semantic( new SopranoModule() );

    sempr::core::IDGenerator::getInstance().setStrategy(
        // std::unique_ptr<sempr::core::UUIDGeneration>( new sempr::core::UUIDGeneration(false) )
        std::unique_ptr<sempr::core::IncrementalIDGeneration>( new sempr::core::IncrementalIDGeneration(storage) )
    );

    sempr::core::Core c(storage);
    c.addModule(active);
    c.addModule(debug);
    c.addModule(updater);
    c.addModule(semantic);


    /********************************
        test query-within-query
    **********************************/
    DummyModule::Ptr module(new DummyModule());
    c.addModule(module);

    DummyQuery::Ptr query(new DummyQuery());
    c.answerQuery(query);
    std::cout << query->count << '\n';

    // add some rdf-stuff
    Person::Ptr p(new Person());
    c.addEntity(p);

    c.answerQuery(query);
    std::cout << query->count << '\n';

}
