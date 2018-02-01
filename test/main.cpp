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

    // add rules if not present
    {
        std::vector<RuleSet::Ptr> rulesets;
        storage->loadAll(rulesets);
        if (rulesets.size() > 0) {
            std::cout << "found ruleset, not creating new" << '\n';
            for (auto rs : rulesets) { rs->loaded(); }
        } else {
            std::cout << "no ruleset found, creating one." << '\n';
            RuleSet::Ptr rules(new RuleSet());
            rules->add("[someRule: (?p rdf:type sempr:Person) -> (?p rdf:type sempr:Human)]");
            c.addEntity(rules);
        }
    }


    LocalCS::Ptr root(new LocalCS());
    LocalCS::Ptr frame(new LocalCS());
    frame->setParent(root);
    frame->setRotation(0, 0, 1, M_PI/2.);
    frame->setTranslation(5, 6, 7);

    GeometryCollection::Ptr coll(new GeometryCollection());
    coll->setCS(frame);

    std::cout << "insert..." << '\n';
    for (size_t i = 0; i < 10000; i++) {
        OGRPoint* pt = (OGRPoint*) OGRGeometryFactory::createGeometry(wkbPointZM);
        pt->setX(i); pt->setY(i);
        coll->geometry()->addGeometryDirectly(pt);
    }

    // print(coll->geometry());
    std::cout << "transform..." << '\n';
    coll->transformToCS(root);
    std::cout << "done." << '\n';
    // print(coll->geometry());

    std::cout << "----------------" << '\n';
    std::cout << "TEST SopranoModule" << '\n';

    auto sq = std::make_shared<SPARQLQuery>();
    // sq->query = "SELECT * WHERE { ?s sempr:age ?age . FILTER (?age > 50) }";
    sq->query = "SELECT * WHERE { ?s rdf:type sempr:Human . }";
    c.answerQuery(sq);
    for (auto r : sq->results) {
        for (auto p : r) {
            std::cout << p.second << " | ";
        }
        std::cout << '\n';
    }


    return 0;
}
