#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"
#include "SimpleIncrementalIDGenerator.hpp"
#include "SeparateFileStorage.hpp"
#include "component/TripleVector.hpp"

#include <boost/filesystem.hpp>
#include <iostream>

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(FileStorageTest)

    // to be used across test. We are testing storage, not id generation.
    const std::string entityID = "Entity_0";

    BOOST_AUTO_TEST_CASE(persist)
    {
        fs::path folder("./db");
        if (fs::exists(folder)) fs::remove_all(folder);

        sempr::SeparateFileStorage::Ptr store;
        // throws as the folder does not exists
        BOOST_CHECK_THROW(store = std::make_shared<sempr::SeparateFileStorage>(folder.string()), sempr::Exception);

        // create the folder
        fs::create_directory(folder);
        // and the storage module
        store = std::make_shared<sempr::SeparateFileStorage>(folder.string());

        // create the core (use store for storage AND idgen!, though we don't test idgen here).
        sempr::Core core(store, store);

        // create an entity, and add a triplevector component
        auto entity = sempr::Entity::create();
        entity->setId(entityID);

        // add to core -> persist
        core.addEntity(entity);

        // entity should be persisted now
        BOOST_CHECK(fs::exists(folder / (entity->id() + ".json")));
    }

    BOOST_AUTO_TEST_CASE(load)
    {
        fs::path folder("./db");
        BOOST_CHECK(fs::exists(folder / (entityID + ".json"))); // check if the entity was persisted. precondition for this test...

        auto store = std::make_shared<sempr::SeparateFileStorage>(folder.string());
        auto entity = store->load(entityID);

        BOOST_CHECK(entity != nullptr);

        sempr::Core core(store, store);
        core.addEntity(entity); // triggers persistence... the architecture is not very clever, yet, when it comes to persistence.
    }

    BOOST_AUTO_TEST_CASE(remove)
    {
        fs::path folder("./db");
        auto store = std::make_shared<sempr::SeparateFileStorage>(folder.string());

        auto entities = store->loadAll();
        BOOST_CHECK(entities.size() > 0); // precondition for the remove test: there have to be entities to remove

        // directly test the remove of the store, not going through the core here.
        for (auto e : entities)
        {
            store->remove(e);
        }

        entities = store->loadAll();
        BOOST_CHECK(entities.size() == 0);
    }

BOOST_AUTO_TEST_SUITE_END()
