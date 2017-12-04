#include <sempr/sempr.hpp>
using namespace sempr::storage;

#include <sempr/entity/CoffeeMug.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
using namespace sempr::entity;

#include <sempr/processing/DebugModule.hpp>
#include <sempr/processing/DBUpdateModule.hpp>
#include <sempr/processing/ActiveObjectStore.hpp>
using namespace sempr::processing;

#include <fstream>
#include <iostream>

#include <odb/database.hxx>
#include <Person_odb.h>

#include <sempr/core/IncrementalIDGeneration.hpp>

int main(int argc, char** args)
{
    ODBStorage::Ptr storage( new ODBStorage() );
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
        Person::Ptr p(new Person());
        c.addEntity(p);
        Person::Ptr p2(new Person());
        c.addEntity(p2);


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
            std::cout << "Person id: " << p->id() << std::endl;

            // add a year.
            p->age(p->age()+1);
            std::cout << "Person: "
                << p->name() << ", "
                << p->age() << " years old, "
                << p->height() << "m." << std::endl;
        }
    }


    return 0;
}
