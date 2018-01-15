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

#include <RuleSet_odb.h>

#include <sempr/core/IncrementalIDGeneration.hpp>

// small, easy, specialized query for persons over the age of 55.
class PersonQuery : public ObjectQueryBase {
public:
    std::vector<Person::Ptr> results;
    void consider(DBObject::Ptr o) override
    {
        auto ptr = std::dynamic_pointer_cast<Person>(o);
        if (ptr)
        {
            if (ptr->age() >= 55) {
                results.push_back(ptr);
            }
        }
    }
};



int main(int argc, char** args)
{
    int numInsert = 1;
    ODBStorage::Ptr storage( new ODBStorage() );
    // ODBStorage::Ptr storage( new ODBStorage(":memory:") );

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


    {
        // insert.
        for (int i = 0; i < numInsert; i++) {
            Person::Ptr p(new Person());
            c.addEntity(p);
        }
        // Person::Ptr p2(new Person());
        // c.addEntity(p2);


        // retrieve

        // std::vector<DBObject::Ptr> objects;
        // storage->loadAll(objects);
        // for (auto o : objects) {
        //     Person::Ptr p = std::dynamic_pointer_cast<Person>(o);
        //     if (p) {
        //         p->loaded(); // no changed-events before announcement!
        //         std::cout << "Person id: " << p->id() << std::endl;
        //
        //         // add a year.
        //         p->age(p->age()+1);
        //         std::cout << "Person: "
        //             << p->name() << ", "
        //             << p->age() << " years old, "
        //             << p->height() << "m." << std::endl;
        //     }
        // }

        std::vector<Person::Ptr> persons;
        storage->loadAll(persons);
        for (auto p : persons) {
            p->loaded(); // no changed-events before announcement!
            // std::cout << "Person id: " << p->id() << std::endl;

            // add a year.
            p->age(p->age()+1);
            std::cout
                << p->id() << ": "
                << p->name() << ", "
                << p->age() << " years old, "
                << p->height() << "m." << std::endl;
        }
    }

    active->printStats();

    // auto q = std::make_shared<ObjectQuery<Person> >();
    auto q = std::make_shared<PersonQuery>();
    c.answerQuery(q);
    std::cout << "query results: " << q->results.size() << '\n';
    for (auto p : q->results)
    {
        std::cout << p->id() << ", age: " << p->age() << '\n';
    }

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
