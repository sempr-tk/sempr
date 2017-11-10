#define BOOST_TEST_MODULE sempr-test
#include <boost/test/included/unit_test.hpp>

#include <odb/core.hxx>
#include <sempr/sempr.hpp>
#include <sempr/entity/Person.hpp>
#include <Person_odb.h>

using namespace sempr::core;
using namespace sempr::storage;
using namespace sempr::processing;
using namespace sempr::entity;


#define DISCRIMINATOR(T) (odb::object_traits_impl<T, odb::id_common>::info.discriminator)

BOOST_AUTO_TEST_SUITE(general_tests)

  BOOST_AUTO_TEST_CASE(core_test)
  {
    ODBStorage::Ptr storage( new ODBStorage() );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );

    Core core(storage);
    core.addModule(active);
    core.addModule(updater);

    // TODO: write real test cases.
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(entity_within_entity_tests)

BOOST_AUTO_TEST_CASE(insertion)
{

  // insertion
  {
    // setup. database will be reset (true)
    ODBStorage::Ptr storage( new ODBStorage("test_sqlite.db", true) );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );

    Core core(storage);
    core.addModule(updater);

    // create  a person, which creates an rdf-entity within
    Person::Ptr p(new Person());
    // save --> persist the person. check 1: we should not get a foreign-key-
    // constraint-violation if the rdf-entity is persisted before the person.
    core.addEntity(p);
  }
}

BOOST_AUTO_TEST_CASE(retrieval)
{
    // retrieval. New ODBStorage, new session, force to load from database
  {
    ODBStorage::Ptr storage( new ODBStorage("test_sqlite.db") );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    DebugModule::Ptr debug( new DebugModule() );

    Core core(storage);
    core.addModule(updater);
    core.addModule(debug);

    // create  a person, which creates an rdf-entity within
    Person::Ptr person(new Person());
    // save --> persist the person. check 1: we should not get a foreign-key-
    // constraint-violation if the rdf-entity is persisted before the person.
    core.addEntity(person);

    boost::uuids::uuid id = person->uuid();

    DBObject::Ptr p = storage->load(id);


    // check if the object is of correct type by using the discriminator.
    BOOST_CHECK_EQUAL(p->discriminator(), DISCRIMINATOR(Person));

    // load all entites, check if there are exactly 2 (person and its RDF).
    std::vector<DBObject::Ptr> all;
    storage->loadAll(all);
    BOOST_CHECK_EQUAL(all.size(), 2);
  }
}

BOOST_AUTO_TEST_SUITE_END()
