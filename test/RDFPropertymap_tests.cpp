#include "test_utils.hpp"
using namespace testing;

/**
    a series of tests to check the functionality of the RDFPropertyMap
    which is used for easy storage of primitive datatypes and object-pointers
    in an RDFEntity -- and therefore useable for inference.
*/
BOOST_AUTO_TEST_SUITE(entity_RDFPropertyMap)

    // to keep between tests.
    std::string mapId, personId;
    std::string databaseFile = "test_rdf_sqlite.db";

    BOOST_AUTO_TEST_CASE(propertymap_insertion)
    {
        ODBStorage::Ptr storage = setUpStorage(databaseFile, true);
        DBUpdateModule::Ptr updater(new DBUpdateModule(storage));

        Core core;
        core.addModule(updater);

        // create an entity and assign different values
        RDFPropertyMap::Ptr map(new RDFPropertyMap("subject", "http://baseURI/"));
        core.addEntity(map);
        mapId = map->id();


        RDFPropertyMap& m = *map;
        m["int"] = 42;
        BOOST_CHECK( !m.hasProperty("float") );
        m["float"] = 1.234f;
        BOOST_CHECK( m.hasProperty("float") );
        m["string"] = "Hello, World!";

        // create another entity and point to it.
        Person::Ptr person(new Person());
        core.addEntity(person);
        personId = person->id();

        m["person"] = person;

        map->changed();

        updater->updateDatabase();
    }


    BOOST_AUTO_TEST_CASE(propertymap_retrieval)
    {
        // load the values that were stored in the previous test and check for
        // equality. also, test some conversions.
        ODBStorage::Ptr storage = loadStorage(databaseFile);
        Core core;
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
        BOOST_CHECK_EQUAL(personId, person->id());

        // test removal of properties
        BOOST_CHECK( m.hasProperty("float") );
        m.removeProperty("float");
        BOOST_CHECK( !m.hasProperty("float") );

        // test invalid accesses
        BOOST_CHECK_THROW(
            RDFPropertyMap::Ptr foo = m["person"], // person != RDFPropertyMap
            std::runtime_error
        );
        BOOST_CHECK_THROW(
            Entity::Ptr foo = m["no-entity-here"],
            std::runtime_error
        );

        removeStorage(databaseFile);
    }
BOOST_AUTO_TEST_SUITE_END()
