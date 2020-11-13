#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"
#include "component/GeosGeometry.hpp"

#include <rete-reasoner/RuleParser.hpp>
#include <rete-rdf/Triple.hpp>

#include <fstream>

bool containsTriple(rete::Reasoner reasoner,
                    const std::string& s, const std::string& p, const std::string& o)
{
    auto wmes = reasoner.getCurrentState().getWMEs();
    for (auto wme : wmes)
    {
        auto triple = std::dynamic_pointer_cast<rete::Triple>(wme);
        if (triple &&
            triple->subject == s &&
            triple->predicate == p &&
            triple->object == o)
        {
            return true;
        }
    }
    return false;
}


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


    BOOST_AUTO_TEST_CASE(area_of_geom)
    {
        sempr::Core core;
        core.loadPlugins("src");
        core.loadPlugins("../src");

        core.addRules(
            "[EC<GeosGeomI>(?entity ?geometry), geo:area(?area ?geometry) "
            "-> (?entity <hasArea> ?area)]"
        );

        auto entity = sempr::Entity::create();
        auto geom = std::make_shared<sempr::GeosGeometry>();
        entity->addComponent(geom);


        const geos::geom::GeometryFactory& factory =
                *geos::geom::GeometryFactory::getDefaultInstance();
        geos::io::WKTReader reader(factory);

        // parse the string
        std::unique_ptr<geos::geom::Geometry> g(reader.read(
              "POLYGON(("
              "0 2,"
              "3 2,"
              "3 0,"
              "0 0,"
              "0 2))"
        ));

        geom->setGeometry(std::move(g));

        core.addEntity(entity);
        core.performInference();
        std::ofstream("geo_area.dot") << core.reasoner().net().toDot();

        BOOST_CHECK(
            containsTriple(core.reasoner(),
                "<sempr:" + entity->id() + ">", "<hasArea>", std::to_string(6.))
        );
    }

BOOST_AUTO_TEST_SUITE_END()
