#include <sempr/sempr.hpp>
using namespace sempr::storage;

#include <sempr/entity/CoffeeMug.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
using namespace sempr::entity;

#include <sempr/processing/DebugModule.hpp>
#include <sempr/processing/DBUpdateModule.hpp>
#include <sempr/processing/ActiveObjectStore.hpp>
using namespace sempr::processing;

#include <sempr/query/ObjectQuery.hpp>
using namespace sempr::query;

#include <fstream>
#include <iostream>

#include <odb/database.hxx>
#include <Person_odb.h>

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

    if (argc > 1) numInsert = atoi(args[1]);
    if (argc > 2) {
        storage.reset();
        storage.reset(new ODBStorage(":memory:"));
    }

/* Performance results for "insert X persons, load them all, increment age"
1. In-Memory-Database, X = 10000

    time ./test/manual_test 10000 mem > /dev/null
    real    0m3.994s
    user    0m3.976s
    sys     0m0.016s

2. File-Database (sqlite3), X = 10000

    time ./test/manual_test 10000 > /dev/null
    real    13m6.421s
    user    0m23.488s
    sys     0m32.204s
*/



    // ODBStorage::Ptr storage( new ODBStorage(":memory:") );
    // ODBStorage::Ptr storage( new ODBStorage() );
    DebugModule::Ptr debug( new DebugModule() );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );

    sempr::core::IDGenerator::getInstance().setStrategy(
        // std::unique_ptr<sempr::core::UUIDGeneration>( new sempr::core::UUIDGeneration(false) )
        std::unique_ptr<sempr::core::IncrementalIDGeneration>( new sempr::core::IncrementalIDGeneration(storage) )
    );

    sempr::core::Core c(storage);
    c.addModule(active);
    // c.addModule(debug);
    c.addModule(updater);


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

    return 0;
}
