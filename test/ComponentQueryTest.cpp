#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include <rete-reasoner/RuleParser.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"
#include "component/TriplePropertyMap.hpp"
#include "component/GeosGeometry.hpp"
#include "plugins/RDFPlugin.hpp"

BOOST_AUTO_TEST_SUITE(ComponentQueryTest)

    BOOST_AUTO_TEST_CASE(queryTest)
    {
        sempr::Core core;
        core.loadPlugins("../src");
        core.loadPlugins("src");


        // add 2 entities, everything inferred!
        // one with a larger geometry then the other.
        // Rules that classify an entity as "big" or "small" depending on a
        // size threshold.

        core.addRules(
            "@PREFIX ex: <ex:>\n"
            "[true(),"
            " geo:fromWKT(?point \"Point(0 0)\"),"
            " geo:buffer(?circle ?point 10),"
            " createEntity(?entity ex:field1)"
            " ->"
            " EC(?entity ?circle),"
            " (?entity ex:type ex:Field)]");

        core.addRules(
            "@PREFIX ex: <ex:>\n"
            "[true(),"
            " geo:fromWKT(?point \"Point(100 0)\"),"
            " geo:buffer(?circle ?point 20),"
            " createEntity(?entity ex:field2)"
            " ->"
            " EC(?entity ?circle),"
            " (?entity ex:type ex:Field)]");

        core.addRules(
            "@PREFIX ex: <ex:>\n"
            "[EC<GeosGeomI>(?e ?g), geo:area(?area ?g), ge(?area 400) -> (?e ex:size ex:big)]"
            "[EC<GeosGeomI>(?e ?g), geo:area(?area ?g), lt(?area 400) -> (?e ex:size ex:small)]"
        );

        core.reasoner().performInference();

        std::ofstream("test_component_query.dot") << core.reasoner().net().toDot();

        auto plugin = core.getPlugin<sempr::RDFPlugin>();
        BOOST_REQUIRE(plugin);


        {
            // query for "big" entities, and get their geometry component
            auto results = plugin->componentQuery(
                    "SELECT * WHERE { ?f <ex:size> <ex:big> . }")
                .with<sempr::GeosGeometry>("f").includeInferred(true)
                .execute();

            // only one of the entities is "big" ...
            BOOST_REQUIRE(results.size() == 1);

            auto& r = results[0];
            auto sparqlResult = std::get<0>(r);
            // ... and it is "field2"
            BOOST_CHECK(sparqlResult["f"].second == "ex:field2");

            // now access the geometry and make sure it is the correct one by
            // checking the size constraint again
            auto geo = std::get<1>(r);
            BOOST_CHECK(geo.component->geometry()->getArea() >= 400);
        }

        // leaving out the inferred components, there should be no result
        {
            auto results = plugin->componentQuery(
                    "SELECT * WHERE { ?f <ex:size> <ex:big> . }")
                .with<sempr::GeosGeometry>("f")
                .execute();
            BOOST_CHECK(results.size() == 0);
        }

        // but if the component is optional, there should be a result with
        // a nullptr geometry
        {
            auto results = plugin->componentQuery(
                    "SELECT * WHERE { ?f <ex:size> <ex:big> . }")
                .with<sempr::GeosGeometry>("f").optional(true)
                .execute();
            BOOST_REQUIRE(results.size() == 1);
            auto& r = results[0];
            auto geo = std::get<1>(r);
            BOOST_CHECK(geo.component == nullptr);
            BOOST_CHECK(geo.entity == nullptr);
            BOOST_CHECK(!geo);
        }
    }

    BOOST_AUTO_TEST_CASE(queryTest2)
    {
        sempr::Core core;
        core.loadPlugins("../src");
        core.loadPlugins("src");

        core.addRules(
            "@PREFIX ex: <ex:>\n"
            "[true() -> (ex:SubClass1 ex:subClassOf ex:Class1), (ex:SubClass2 ex:subClassOf ex:SubClass1)]"
            "[(?a ex:subClassOf ?b), (?b ex:subClassOf ?c) -> (?a ex:subClassOf ?c)]"
            "[(?a ex:subClassOf ?b), (?c ex:type ?a) -> (?c ex:type ?b)]"
        );

        // this time, test with a manually asserted entity and component

        auto e1 = sempr::Entity::create();
        auto tmap = std::make_shared<sempr::TriplePropertyMap>();
        tmap->map_["ex:type"] = { "ex:SubClass2", true };
        e1->addComponent(tmap);

        core.addEntity(e1);
        core.performInference();

        auto plugin = core.getPlugin<sempr::RDFPlugin>();
        BOOST_REQUIRE(plugin);

        // query for a component that is not exactly what was added, but
        // super-type. (TriplePropertyMap inherits from TripleContainer)
        auto results =
            plugin->componentQuery(
                    "SELECT * WHERE { ?e <ex:type> <ex:Class1> . }")
                .with<sempr::TripleContainer>("e")
                .execute();

        BOOST_REQUIRE(results.size() == 1);
        auto& r = results[0];

        // print the contents of the container... not a real test, but
        // nice to see once. ;)
        auto container = std::get<1>(r);
        for (auto& entry : *container.component)
        {
            std::cout << entry.toString() << std::endl;
        }
    }



    BOOST_AUTO_TEST_CASE(filterbytag)
    {
        sempr::Core core;
        core.loadPlugins("../src");
        core.loadPlugins("src");

        auto e1 = sempr::Entity::create();
        auto c1 = std::make_shared<sempr::TriplePropertyMap>();
        auto c2 = std::make_shared<sempr::TriplePropertyMap>();

        c1->map_["ex:foo"] = "bar";
        c2->map_["ex:foo"] = "baz";

        e1->addComponent(c1, "bar-tag");
        e1->addComponent(c2, "baz-tag");

        core.addEntity(e1);
        core.performInference();

        std::ofstream("ComponentQueryTest_filterbytag.dot") << core.reasoner().net().toDot();

        auto plugin = core.getPlugin<sempr::RDFPlugin>();
        BOOST_REQUIRE(plugin);

        {
            auto results = plugin->componentQuery(
                    "SELECT DISTINCT ?a WHERE { ?a <ex:foo> ?b . }")
                .with<sempr::TriplePropertyMap>("a")
                .execute();

            std::cout << results.size() << std::endl;

            for (auto& r : results)
            {
                auto b = std::get<0>(r);
                std::cout << "{";
                for (auto& binding : b)
                {
                    std::cout << binding.first << " : " << binding.second.second << ", ";
                }
                std::cout << "} , " << rete::util::demangle(typeid(std::get<1>(r)).name()) << ", " << std::get<1>(r).tag << std::get<1>(r).component << std::endl;
            }

            BOOST_CHECK(results.size() == 2);
        }

        {
            auto results = plugin->componentQuery(
                    "SELECT DISTINCT ?a WHERE { ?a <ex:foo> ?b . }")
                .with<sempr::TriplePropertyMap>("a").withTag("bar-tag")
                .execute();
            BOOST_REQUIRE(results.size() == 1);
            auto& r = results[0];

            auto tpm = std::get<1>(r);
            BOOST_CHECK(tpm.tag == "bar-tag");
            std::string val = tpm.component->map_["ex:foo"];
            BOOST_CHECK(val == "bar");
        }
    }

BOOST_AUTO_TEST_SUITE_END()
