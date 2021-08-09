#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "Core.hpp"
#include <rete-reasoner/ExplanationToJSONVisitor.hpp>
#include "ECWMEToJSONConverter.hpp"
#include "TupleWMEToJSONConverter.hpp"
#include <rete-rdf/Triple.hpp>


using namespace sempr;

BOOST_AUTO_TEST_SUITE(ExplainECWMEJSONTest)
    BOOST_AUTO_TEST_CASE(test1)
    {
        sempr::Core core;
        core.loadPlugins("../src");
        core.loadPlugins("src");

        core.addRules(
R"foo(
    [true() -> (<foo> <bar> <baz>)]
    [
        (?x <bar> <baz>),
        {
            geo:fromWKT(?geo "POINT Z (1 2 3)"),
            createEntity(?entity ?x)
            /* Create an entity {entity} and a geometry {geo} */
        }
        ->
        {
            EC(?entity ?geo "some tag")
            /* Infer that {geo} is part of {entity} */
        }
    ]
    [
        EC<GeosGeomI>(?entity ?geometry ?tag)
        ->
        (?entity <has-a> <geometry>)
    ]
)foo"
        );

        core.performInference();

        {
            std::ofstream("ExplanationJSONTest_network.dot") << core.reasoner().net().toDot();
        }

        auto infstate = core.reasoner().getCurrentState();
        auto wme = std::make_shared<rete::Triple>("<foo>", "<has-a>", "<geometry>");

        rete::ExplanationToJSONVisitor visitor;
        visitor.addToJSONConverter(std::make_shared<ECWMEToJSONConverter>());
        visitor.addToJSONConverter(std::make_shared<TupleWMEToJSONConverter>());

        infstate.traverseExplanation(wme, visitor);
        {
            std::ofstream("ExplanationJSONTest_explanation.json") << visitor.json().dump();
        }
    }
BOOST_AUTO_TEST_SUITE_END()

