#define BOOST_TEST_MODULE sempr-test
#include <boost/test/included/unit_test.hpp>

#include <odb/core.hxx>
#include <sempr/sempr.hpp>
#include <sempr/entity/Person.hpp>
#include <Person_odb.h>

#include <sempr/entity/RDFPropertyMap.hpp>

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
    // DebugModule::Ptr debug( new DebugModule() );

    Core core(storage);
    core.addModule(updater);
    // core.addModule(debug);

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


/**
    a series of tests to check the functionality of the RDFPropertyMap
    which is used for easy storage of primitive datatypes and object-pointers
    in an RDFEntity -- and therefore useable for inference.
*/
BOOST_AUTO_TEST_SUITE(entity_RDFPropertyMap)

    // to keep between tests.
    boost::uuids::uuid mapId, personId;

    BOOST_AUTO_TEST_CASE(propertymap_insertion)
    {
        ODBStorage::Ptr storage( new ODBStorage("test_sqlite.db", true) );
        DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );

        Core core(storage);
        core.addModule(updater);

        // create an entity and assign different values
        RDFPropertyMap::Ptr map(new RDFPropertyMap("subject", "http://baseURI/"));
        core.addEntity(map);
        mapId = map->uuid();


        RDFPropertyMap& m = *map;
        m["int"] = 42;
        m["float"] = 1.234f;
        m["string"] = "Hello, World!";

        // create another entity and point to it.
        Person::Ptr person(new Person());
        core.addEntity(person);
        personId = person->uuid();

        m["person"] = person;
    }


    BOOST_AUTO_TEST_CASE(propertymap_retrieval)
    {
        // load the values that were stored in the previous test and check for
        // equality. also, test some conversions.
        ODBStorage::Ptr storage( new ODBStorage("test_sqlite.db") );
        Core core(storage);
        DBObject::Ptr tmp = storage->load(mapId);

        // cast should succeed.
        auto map = std::dynamic_pointer_cast<RDFPropertyMap>(tmp);
        BOOST_CHECK(map);

        // check values:
        RDFPropertyMap& m = *map;
        int i = m["int"];
        BOOST_CHECK_EQUAL(i, 42);
        float f = m["float"];
        BOOST_CHECK_CLOSE(f, 1.234f, 0.00001);
        std::string s = m["string"];
        BOOST_CHECK_EQUAL(s, "Hello, World!");
        Person::Ptr person = m["person"];
        BOOST_CHECK_EQUAL(personId, person->uuid());

        // test invalid accesses
        BOOST_CHECK_THROW(
            RDFPropertyMap::Ptr foo = m["person"], // person != RDFPropertyMap
            std::runtime_error
        );
        BOOST_CHECK_THROW(
            Entity::Ptr foo = m["no-entity-here"],
            std::runtime_error
        );

    }
BOOST_AUTO_TEST_SUITE_END()
