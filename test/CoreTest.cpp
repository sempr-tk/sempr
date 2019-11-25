#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"


BOOST_AUTO_TEST_SUITE(CoreTest)

    BOOST_AUTO_TEST_CASE(add_entity)
    {
        sempr::Core core;
        auto entity = sempr::Entity::create();
        core.addEntity(entity);

        // entity now has an id
        BOOST_CHECK(not entity->id().empty());
    }

    BOOST_AUTO_TEST_CASE(multi_add)
    {
        sempr::Core core;
        auto entity = sempr::Entity::create();
        core.addEntity(entity);

        BOOST_CHECK_THROW(core.addEntity(entity), sempr::Exception);
    }

    BOOST_AUTO_TEST_CASE(add_remove)
    {
        sempr::Core core;
        auto entity = sempr::Entity::create();
        core.addEntity(entity);
        core.removeEntity(entity);
    }
BOOST_AUTO_TEST_SUITE_END()
