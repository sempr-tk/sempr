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
#include <boost/uuid/uuid.hpp>

#include <odb/database.hxx>

int main(int argc, char** args)
{
    ODBStorage::Ptr storage( new ODBStorage() );
    DebugModule::Ptr debug( new DebugModule() );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );

    sempr::core::Core c(storage);
    c.addModule(active);
    c.addModule(debug);
    c.addModule(updater);


    {
        // insert.
        Person::Ptr p(new Person());
        c.addEntity(p);


        // retrieve

        std::vector<DBObject::Ptr> objects;
        storage->loadAll(objects);
        for (auto o : objects) {
            Person::Ptr p = std::dynamic_pointer_cast<Person>(o);
            if (p) {
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
    }


    return 0;
}
