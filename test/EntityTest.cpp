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
        auto entity = std::make_shared<Entity>();
        auto component = std::make_shared<Component>();

        entity->addComponent(component);
    }
BOOST_AUTO_TEST_SUITE_END()
