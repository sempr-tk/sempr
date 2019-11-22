#include "test_utils.hpp"
using namespace testing;

#include <sempr/entity/example/PropertyTestEntity.hpp>
#include <PropertyTestEntity_odb.h>

#include <sempr/processing/SopranoModule.hpp>
#include <sempr/processing/DBUpdateModule.hpp>

/**
    a series of tests to check the functionality of the RDFPropertyMap
    which is used for easy storage of primitive datatypes and object-pointers
    in an RDFEntity -- and therefore useable for inference.
*/
BOOST_AUTO_TEST_SUITE(entity_SemanticEntity)

    // to keep between tests.
    std::string mapId, personId;
    std::string databaseFile = "test_sqlite.db";

    BOOST_AUTO_TEST_CASE(semantic_entity_test)
    {
        auto storage = setUpStorage(databaseFile, false);
        DBUpdateModule::Ptr updater(new DBUpdateModule(storage));
        ActiveObjectStore::Ptr active(new ActiveObjectStore());
        SopranoModule::Ptr soprano(new SopranoModule());


        Core core;
        core.addModule(updater);
        core.addModule(active);
        core.addModule(soprano);

        Person::Ptr person(new Person());
        core.addEntity(person);

        PropertyTestEntity::Ptr entity(new PropertyTestEntity());
        core.addEntity(entity);

        entity->intValue_ = 42;
        entity->floatValue_ = 3.14f;
        entity->doubleValue_ = 3.1415926535897932384626433;
        entity->stringValue_ = "Hello, World!";
        entity->entityValue_ = person;

        entity->changed();

        // updater->updateDatabase();


        // since the PropertyTestEntity derives from SemanticEntity we can iterate over its triples.
        int num = 0;
        for (auto t : *entity) num++;
        // expect 5 triples: int, float, double, string, entity
        // but they all are registered twice, once with and once without type information
        BOOST_CHECK_EQUAL(num, 10);

        // debug: print triples:
        for (auto t : *entity) {
            std::cout << t.subject << " " << t.predicate << " " << t.object << " " << t.document << std::endl;
        }
    }


    BOOST_AUTO_TEST_CASE(semantic_entity_test_cleanup)
    {
        removeStorage(databaseFile);
    }
BOOST_AUTO_TEST_SUITE_END()
