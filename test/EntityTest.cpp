#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"
#include "Component.hpp"

using namespace sempr;

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
        BOOST_CHECK(l2[0] == component);
    }

    BOOST_AUTO_TEST_CASE(add_component_twice)
    {
        auto entity = Entity::create();
        auto component = std::make_shared<Component>();

        // component added to one entity twice
        entity->addComponent(component);
        BOOST_CHECK_THROW(
                entity->addComponent(component), 
                sempr::Exception);

        // component added to a second entity
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
        BOOST_CHECK(entity->getComponents<Component>()[0] == c1);

        BOOST_CHECK_THROW(
                entity->removeComponent(c2),
                sempr::Exception);
    }
BOOST_AUTO_TEST_SUITE_END()

