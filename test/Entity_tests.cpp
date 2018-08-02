#include "test_utils.hpp"
using namespace testing;


BOOST_AUTO_TEST_SUITE(entity_Entity)

    std::string databaseFile = "entity_tests_sqlite.db";
    std::string parentId;

    BOOST_AUTO_TEST_CASE(entity_registerchild_insert)
    {
        ODBStorage::Ptr storage = setUpStorage(databaseFile, true);
        DBUpdateModule::Ptr updater(new DBUpdateModule(storage));

        Core core;
        core.addModule(updater);

        // create an entity and assign different values
        Entity::Ptr e1(new Entity());
        Entity::Ptr e2(new Entity());
        core.addEntity(e1);
        core.addEntity(e2);

        parentId = e1->id();

        e1->registerChildEntity(e2);
        e1->changed();

        updater->updateDatabase();
    }

    BOOST_AUTO_TEST_CASE(entity_registerchild_load_and_remove)
    {
        ODBStorage::Ptr storage = loadStorage(databaseFile);
        DBUpdateModule::Ptr updater(new DBUpdateModule(storage));

        Core core;
        core.addModule(updater);

        std::vector<Entity::Ptr> entities;
        storage->loadAll(entities);
        for (auto e : entities) e->loaded();

        BOOST_CHECK_EQUAL(entities.size(), 4); // 2 Entities, 1 IncIDGenConfig, 1 PrefixInfo_Entity

        auto e1 = storage->load<Entity>(parentId);
        core.removeEntity(e1);

        updater->updateDatabase();
    }


    BOOST_AUTO_TEST_CASE(entity_registerchild_checkclean)
    {
        ODBStorage::Ptr storage = loadStorage(databaseFile);
        Core core;

        // check if both entities were removed
        std::vector<Entity::Ptr> entities;
        storage->loadAll(entities);
        for (auto e : entities) { e->loaded(); std::cout << e->id() << std::endl; }

        BOOST_CHECK_EQUAL(entities.size(), 2); // 2 less than before


        removeStorage(databaseFile);
    }
BOOST_AUTO_TEST_SUITE_END()
