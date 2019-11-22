#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "sempr/core/Core.hpp"
#include "sempr/entity/Entity.hpp"
#include "sempr/core/Exception.hpp"


BOOST_AUTO_TEST_SUITE(CoreTest)

    BOOST_AUTO_TEST_CASE(add_entity)
    {
        sempr::core::Core core;
        auto entity = std::make_shared<sempr::entity::Entity>();
        core.addEntity(entity);

        // entity now has an id
        BOOST_CHECK(not entity->id().empty());
    }

    BOOST_AUTO_TEST_CASE(multi_add)
    {
        sempr::core::Core core;
        auto entity = std::make_shared<sempr::entity::Entity>();
        core.addEntity(entity);

        BOOST_CHECK_THROW(core.addEntity(entity), sempr::Exception);
    }

    BOOST_AUTO_TEST_CASE(add_remove)
    {
        sempr::core::Core core;
        auto entity = std::make_shared<sempr::entity::Entity>();
        core.addEntity(entity);
        core.removeEntity(entity);
    }
BOOST_AUTO_TEST_SUITE_END()
