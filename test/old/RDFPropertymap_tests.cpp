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

        // conversions from/to strings are sometimes a bit buggy, at a compiler level.
        // these are a few tests added for issue #30
        std::string foo = m["string"]; // was no problem. copy constructor of string is used
        std::string bar;
        // bar = m["string"]; // this won't compile, the assignment operator is ambiguous here
        bar = m["string"].get<std::string>(); // but this works as the desired type is stated explicitely
        BOOST_CHECK_EQUAL(bar, "Hello, World!");
        // another problem was the fact that the operator == wasn't implemented. This should be okay now, too. The comments describe the error before implementing the operator.
        bool ok;
        ok = m["string"] == std::string("Hello, World!");  // invalid operands to binary expression
        BOOST_CHECK(ok);

        ok = m["string"] == "Hello, World!"; // interestingly, this was only a warning but seems to work!
        BOOST_CHECK(ok);
        ok = m["string"] == "Something wrong.";
        BOOST_CHECK(!ok);
        ok = m["int"] == 42; // invalid operands to binary expression
        BOOST_CHECK(ok);
        ok = m["float"] == 1.234f;
        BOOST_CHECK(ok);
        // ----------------
        BOOST_CHECK( (m["int"] < 100) );
        BOOST_CHECK( (m["int"] >  40) );
        BOOST_CHECK( (m["int"] <= 42) );
        BOOST_CHECK( (m["int"] <= 43) );
        BOOST_CHECK(!(m["int"] <= 10) );
        BOOST_CHECK( (m["int"] >= 10) );
        BOOST_CHECK( (m["int"] !=  7) );
        BOOST_CHECK(!(m["int"] != 42) );
        BOOST_CHECK( (m["string"] < "ZZZ") );
        BOOST_CHECK( (m["string"] > "AAA") );
        BOOST_CHECK(!(m["string"] != "Hello, World!") );




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
