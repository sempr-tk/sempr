#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include <fstream>
#include <cstdio>

#include <rete-reasoner/Reasoner.hpp>
#include <rete-reasoner/AssertedEvidence.hpp>
#include <rete-reasoner/RuleParser.hpp>
#include <rete-rdf/Triple.hpp>

#include "Core.hpp"
#include "Exception.hpp"
#include "Entity.hpp"
#include "nodes/ConstructRulesBuilder.hpp"
#include "nodes/TextComponentTextBuilder.hpp"
#include "nodes/ECNodeBuilder.hpp"
#include "component/TextComponent.hpp"

using namespace sempr;

bool contains(const std::vector<rete::WME::Ptr>& wmes,
              const std::string& subject,
              const std::string& predicate,
              const std::string& object)
{
    for (auto& wme : wmes)
    {
        auto triple = std::dynamic_pointer_cast<rete::Triple>(wme);
        if (!triple) continue;

        if (triple->subject == subject &&
            triple->predicate == predicate &&
            triple->object == object)
        {
            return true;
        }
    }
    return false;
}


BOOST_AUTO_TEST_SUITE(ConstructRulesEffectTest)
    BOOST_AUTO_TEST_CASE(test1)
    {
        sempr::Core core;

        core.parser().registerNodeBuilder<ConstructRulesBuilder>(&core);
        core.parser().registerNodeBuilder<ECNodeBuilder<TextComponent>>();
        core.parser().registerNodeBuilder<TextComponentTextBuilder>();

        core.addRules(
            "[EC<TextComponent>(?e ?c), text:value(?text ?c) -> constructRules(?text)]"
        );


        {
            std::ofstream("ConstructRulesEffectTest_0.dot") << core.reasoner().net().toDot();
        }

        core.performInference();

        auto wmes = core.reasoner().getCurrentState().getWMEs();

        BOOST_CHECK(wmes.empty());

        // add an entity with a rule definition that unconditionally infers something
        auto entity = Entity::create();
        auto text = std::make_shared<TextComponent>("[true() -> (<x> <y> <z>)]");
        entity->addComponent(text);

        core.addEntity(entity);

        core.performInference();

        {
            std::ofstream("ConstructRulesEffectTest_1.dot") << core.reasoner().net().toDot();
        }

        wmes = core.reasoner().getCurrentState().getWMEs();

        // check if the new rule has been evaluated already
        BOOST_CHECK(contains(wmes, "<x>", "<y>", "<z>"));


        // now, remove the entity
        core.removeEntity(entity);
        core.performInference();

        {
            std::ofstream("ConstructRulesEffectTest_2.dot") << core.reasoner().net().toDot();
        }

        // make sure the rule is gone, and with it all it inferred
        wmes = core.reasoner().getCurrentState().getWMEs();
        BOOST_CHECK(!contains(wmes, "<x>", "<y>", "<z>"));

    }


    BOOST_AUTO_TEST_CASE(test2)
    {
        sempr::Core core;
        core.parser().registerNodeBuilder<ConstructRulesBuilder>(&core);

        core.addRules(
            "[true() -> constructRules(\"[foo: true() -> constructRules(\\\"[bar: true() -> (<foo> <bar> <baz>)]\\\")]\")]"
        );

        core.performInference();

        {
            std::ofstream("ConstructRulesEffectTest_direct.dot") << core.reasoner().net().toDot();
        }

        auto wmes = core.reasoner().getCurrentState().getWMEs();

        BOOST_CHECK(wmes.size() == 1);

        // check if the new rule has been evaluated already
        BOOST_CHECK(contains(wmes, "<foo>", "<bar>", "<baz>"));
    }
BOOST_AUTO_TEST_SUITE_END()

