#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"
#include "component/GeosGeometry.hpp"

#include <rete-reasoner/RuleParser.hpp>

BOOST_AUTO_TEST_SUITE(GeometryTest)

    BOOST_AUTO_TEST_CASE(geom_in_rule)
    {
        sempr::Core core;
        core.loadPlugins("src");
        core.loadPlugins("../src");

        rete::RuleParser& parser = core.parser();

        auto rules = parser.parseRules(
            "[EC<GeosGeomI>(?entity ?geometry) -> (<foo> <bar> <baz>)]",
            core.reasoner().net()
        );

        auto entity = sempr::Entity::create();
        auto geom = std::make_shared<sempr::GeosGeometry>();
        entity->addComponent(geom);

        core.addEntity(entity);
        core.reasoner().performInference();
        auto wmes = core.reasoner().getCurrentState().getWMEs();

        // there should be 2: ECWME for the entity-component relation, and
        // the (<foo> <bar> <baz>) triple.
        BOOST_CHECK(wmes.size() == 2);
    }

BOOST_AUTO_TEST_SUITE_END()
