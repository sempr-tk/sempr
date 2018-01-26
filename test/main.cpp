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

#include <chrono>


int main(int argc, char** args)
{
  int numInsert = 1;
  bool inmemory = true;

  switch(argc){
  case 2:{
      std::cout << argc << std::endl;
      numInsert = atoi(args[1]);
    }
    break;
  case 3:{
      numInsert = atoi(args[1]);
      inmemory = boost::lexical_cast<bool>(std::atoi(args[2]));
    }
    break;
  default:
    {
      std::cout << "usage: " << std::string(args[0]) << " <numInsert> <inMemory 1|0>" << std::endl;
      return -1;
    }
    break;
  }

  ODBStorage::Ptr storage;

  if (inmemory){
    storage = std::make_shared<ODBStorage>(":memory:");
  }
  else {
    storage = std::make_shared<ODBStorage>();
  }

  DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
  ActiveObjectStore::Ptr active( new ActiveObjectStore() );

  sempr::core::IDGenerator::getInstance().setStrategy(
      // std::unique_ptr<sempr::core::UUIDGeneration>( new sempr::core::UUIDGeneration(false) )
      std::unique_ptr<sempr::core::IncrementalIDGeneration>( new sempr::core::IncrementalIDGeneration(storage) )
  );

  sempr::core::Core c(storage);
  c.addModule(active);
  c.addModule(updater);

  {
    // insert.
    for (int i = 0; i < numInsert; i++) {
        Person::Ptr p(new Person());
        p->age(0);
        c.addEntity(p);
    }
  }

    active->printStats();

  {
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

    return 0;
}
