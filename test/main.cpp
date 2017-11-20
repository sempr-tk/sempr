#include <sempr/sempr.hpp>
using namespace sempr::storage;

#include <sempr/entity/CoffeeMug.hpp>
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
        Person::Ptr person(new Person());
        c.addEntity(person);
    }


    return 0;
}
