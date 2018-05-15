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
      numInsert = atoi(args[1]);
    }
    break;
  case 3:{
      numInsert = atoi(args[2]);
      inmemory = boost::lexical_cast<bool>(std::atoi(args[1]));
    }
    break;
  default:
    {
      std::cout << "usage: " << std::string(args[0]) << " <inMemory 1|0>  <numInsert>" << std::endl;
      return -1;
    }
    break;
  }


  {
    ODBStorage::Ptr storage;
    if (inmemory){
      storage = std::make_shared<ODBStorage>(":memory:",true);
    }
    else {
      storage = std::make_shared<ODBStorage>();
    }

    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    DebugModule::Ptr debug(new DebugModule());
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );

    sempr::core::IDGenerator::getInstance().setStrategy(
        std::unique_ptr<sempr::core::IncrementalIDGeneration>( new sempr::core::IncrementalIDGeneration(storage) )
    );

    sempr::core::Core c;
    c.addModule(active);
    //c.addModule(debug);
    c.addModule(updater);

    auto start = std::chrono::high_resolution_clock::now();
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed;

    // INSERT ONE BY ONE
    {
      std::cout << "insert "<< numInsert << " objects (one-by-one) ... " << std::flush;;
      start = std::chrono::high_resolution_clock::now();
      {
        //INSERT
        for (int i = 0; i < numInsert; i++) {
            Person::Ptr p(new Person());
            p->age(0);
            c.addEntity(p);
        }
      }
      finish = std::chrono::high_resolution_clock::now();
      elapsed = finish - start;
      std::cout << "done in:\t" << elapsed.count() << " s\n";
    }

    // TODO: insert in bulk. not supported yet
    {

    }

    // TODO: update all in bulk
    {
        std::cout << "load all persons ..." << std::flush;
        std::vector<Person::Ptr> persons;
        storage->loadAll(persons);
        for (auto p : persons) { p->loaded(); }
        std::cout << " " << persons.size() << " loaded." << '\n';

        std::cout << "increment every persons age ... " << std::flush;
        start = std::chrono::high_resolution_clock::now();
        for (auto p : persons)
        {
            p->age(p->age() + 1);
            p->changed();
        }
        finish = std::chrono::high_resolution_clock::now();
        elapsed = finish-start;
        std::cout << "done in " << elapsed.count() << " s" << '\n';

        std::cout << "save the changes ... " << std::flush;
        start = std::chrono::high_resolution_clock::now();
        updater->updateDatabase();
        finish = std::chrono::high_resolution_clock::now();
        elapsed = finish-start;
        std::cout << "done in " << elapsed.count() << " s" << '\n';
    }


  }

  return 0;
}
