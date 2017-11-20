#define BOOST_TEST_MODULE sempr-test
#include <boost/test/included/unit_test.hpp>

#include <odb/core.hxx>
#include <sempr/sempr.hpp>
#include <sempr/entity/Person.hpp>
#include <Person_odb.h>
#include <iostream>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

using namespace sempr::core;
using namespace sempr::storage;
using namespace sempr::processing;
using namespace sempr::entity;

#define DISCRIMINATOR(T) (odb::object_traits_impl<T, odb::id_common>::info.discriminator)


ODBStorage::Ptr setUpStorage(std::string db_path, bool reset){

  BOOST_CHECK(!boost::filesystem::exists(db_path) );
  ODBStorage::Ptr storage( new ODBStorage(db_path, reset) );
  BOOST_CHECK(boost::filesystem::exists(db_path) );

  return storage;
}

ODBStorage::Ptr loadStorage(std::string db_path){

  BOOST_CHECK(boost::filesystem::exists(db_path));
  ODBStorage::Ptr storage( new ODBStorage(db_path, false) );

  return storage;
}

void removeStorage(std::string db_path){
  boost::filesystem::remove(db_path);
  BOOST_CHECK(!boost::filesystem::exists(db_path) );
}

void checkEntitiesInStorage(ODBStorage::Ptr storage, int n){
  std::vector<DBObject::Ptr> all;
  storage->loadAll(all);
  BOOST_CHECK_EQUAL(all.size(), n);
}


BOOST_AUTO_TEST_SUITE(general_tests)

  std::string db_path = "test_sqlite.db";

  BOOST_AUTO_TEST_CASE(create_db)
  {
    {
      ODBStorage::Ptr str = setUpStorage(db_path,true);
    }
    {
      ODBStorage::Ptr str = loadStorage(db_path);
    }
    removeStorage(db_path);
  }

  BOOST_AUTO_TEST_CASE(core_test)
  {
    ODBStorage::Ptr storage = setUpStorage(db_path,true);

    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );

    Core core(storage);
    core.addModule(active);
    core.addModule(updater);

    removeStorage(db_path);
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(basic_entity_test)

std::string db_path = "basic_entity_test.db";

BOOST_AUTO_TEST_CASE(insertion)
{
  // insertion
    // setup. database will be reset (true)
    ODBStorage::Ptr storage = setUpStorage(db_path, true);
    Core core(storage);

    // load all entites, check if there are exactly 0 (person and its RDF).
    checkEntitiesInStorage(storage, 0);

    // create  a person, which creates an rdf-entity within
    Person::Ptr p1(new Person());

    // save --> persist the person. check 1: we should not get a foreign-key-
    // constraint-violation if the rdf-entity is persisted before the person.
    core.addEntity(p1);

    // load all entites, check if there are exactly 2 (person and its RDF).
    checkEntitiesInStorage(storage, 2);

    removeStorage(db_path);
}

BOOST_AUTO_TEST_CASE(retrieval){
    // retrieval. New ODBStorage, new session, force to load from database
  boost::uuids::uuid id;
  std::string first = "Peter";
  std::string last = "Müller";
  Person::Gender gender = Person::Gender::MALE;

  {
    ODBStorage::Ptr storage = setUpStorage(db_path, true);

    Core core(storage);

    // create  a person, which creates an rdf-entity within
    Person::Ptr person(new Person());
    // save --> persist the person. check 1: we should not get a foreign-key-
    // constraint-violation if the rdf-entity is persisted before the person.

    person->setFirst(first);
    person->setLast(last);
    person->setGender(gender);

    core.addEntity(person);

    id = person->uuid();
  }

  {
    //core created anew to force a new session, retrieve entity"
    ODBStorage::Ptr storage = loadStorage(db_path);
    Core core(storage);

    DBObject::Ptr p1 = storage->load(id);
    Person::Ptr p2 = storage->load<Person>(id);

    // check if the object is of correct type by using the discriminator.
    BOOST_CHECK_EQUAL(p1->discriminator(), DISCRIMINATOR(Person));

    BOOST_CHECK_EQUAL(p2->getFirst(),first);
    BOOST_CHECK_EQUAL(p2->getLast(),last);
    BOOST_CHECK_EQUAL(p2->getGender(),gender);

    removeStorage(db_path);
  }
}

BOOST_AUTO_TEST_CASE(update) {
  //DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
  //DebugModule::Ptr debug( new DebugModule() );
  //core.addModule(debug);
}

BOOST_AUTO_TEST_CASE(deletion) {
  boost::uuids::uuid id;


  ODBStorage::Ptr storage = setUpStorage(db_path, true);
  Core core(storage);
  Person::Ptr person(new Person());
  core.addEntity(person);
  id = person->uuid();

  // load all entites, check if there are exactly 2 (person and its RDF).
  checkEntitiesInStorage(storage, 2);

  core.removeEntity(person);

  checkEntitiesInStorage(storage, 0);

  removeStorage(db_path);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(register_children_no_duplicates)
    std::string db_path = "test_sqlite.db";

    BOOST_AUTO_TEST_CASE(register_children_no_duplicates_test)
    {
        boost::uuids::uuid personId;
        size_t numBefore;

        {
            ODBStorage::Ptr storage = setUpStorage(db_path, true);
            Core core(storage);
            Person::Ptr person(new Person());
            core.addEntity(person);
            personId = person->uuid();
        }
        {
            ODBStorage::Ptr storage = loadStorage(db_path);
            Core core(storage);

            // load: creates an RDFEntity in the ctor
            Person::Ptr person = storage->load<Person>(personId);

            // get the current number of objects in the database.
            std::vector<DBObject::Ptr> all;
            storage->loadAll(all);
            numBefore = all.size();

            // save: this should *not* add another entity.
            storage->save(person);
        }
        {
            ODBStorage::Ptr storage = loadStorage(db_path);
            Core core(storage);

            checkEntitiesInStorage(storage, numBefore); // <-- should be the same as before.
        }

        removeStorage(db_path);
    }

BOOST_AUTO_TEST_SUITE_END()
