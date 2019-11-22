#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "sempr/core/Core.hpp"
#include "sempr/entity/Entity.hpp"
#include "sempr/core/Exception.hpp"
#include "sempr/entity/Component.hpp"

using namespace sempr;
using namespace sempr::core;
using namespace sempr::entity;

BOOST_AUTO_TEST_SUITE(EntityTest)
    BOOST_AUTO_TEST_CASE(add_component)
    {
        auto entity = Entity::create();
        auto component = std::make_shared<Component>();

        auto l1 = entity->getComponents<Component>();
        BOOST_CHECK(l1.size() == 0);

        entity->addComponent(component);
        auto l2 = entity->getComponents<Component>();
        BOOST_CHECK(l2.size() == 1);
        BOOST_CHECK(l2[0].first == component);
    }

    BOOST_AUTO_TEST_CASE(add_component_tagged)
    {
        auto entity = Entity::create();
        auto component = std::make_shared<Component>();

        auto l1 = entity->getComponents<Component>();
        BOOST_CHECK(l1.size() == 0);

        entity->addComponent(component, "foo");
        auto l2 = entity->getComponents<Component>();
        BOOST_CHECK(l2.size() == 1);
        BOOST_CHECK(l2[0].first == component);
        BOOST_CHECK(l2[0].second == "foo");
    }

    BOOST_AUTO_TEST_CASE(add_component_twice)
    {
        auto entity = Entity::create();
        auto component = std::make_shared<Component>();

        entity->addComponent(component);
        BOOST_CHECK_THROW(
                entity->addComponent(component), 
                sempr::Exception);

        BOOST_CHECK_THROW(
                entity->addComponent(component, "foo"),
                sempr::Exception);

        auto e2 = Entity::create();
        BOOST_CHECK_THROW(
                e2->addComponent(component),
                sempr::Exception);
    }

    BOOST_AUTO_TEST_CASE(remove_component)
    {
        auto entity = Entity::create();
        auto c1 = std::make_shared<Component>();
        auto c2 = std::make_shared<Component>();

        entity->addComponent(c1);
        entity->addComponent(c2);

        BOOST_CHECK(entity->getComponents<Component>().size() == 2);

        entity->removeComponent(c2);
        BOOST_CHECK(entity->getComponents<Component>().size() == 1);
        BOOST_CHECK(entity->getComponents<Component>()[0].first == c1);

        BOOST_CHECK_THROW(
                entity->removeComponent(c2),
                sempr::Exception);
    }
BOOST_AUTO_TEST_SUITE_END()
