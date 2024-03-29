#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"
#include "component/AffineTransform.hpp"
#include "component/GeosGeometry.hpp"
#include "nodes/AffineTransformBuilders.hpp"
#include "nodes/ECNodeBuilder.hpp"
#include "nodes/InferECBuilder.hpp"

#include <rete-reasoner/RuleParser.hpp>
#include <rete-rdf/Triple.hpp>


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

BOOST_AUTO_TEST_SUITE(InferECTest)

    BOOST_AUTO_TEST_CASE(infer)
    {
        rete::RuleParser parser;
        parser.registerNodeBuilder<sempr::ECNodeBuilder<sempr::AffineTransform>>();
        parser.registerNodeBuilder<sempr::ECNodeBuilder<sempr::GeosGeometry>>();
        parser.registerNodeBuilder<sempr::AffineTransformCreateBuilder>();
        parser.registerNodeBuilder<sempr::InferECBuilder>();

        sempr::Core core;

        // for every geos geometry add a affine transform.
        // Useless in its current form, but could be extended to e.g. create
        // default-transformations (identity) for all geometries that have none.
        // One thing preventing this currently is the lack of a component:setTag
        // builtin (which is controversial!)
        auto rules = parser.parseRules(
            "[defaultTF: EC<GeosGeom>(?entity ?geom),"
            "            tf:create(?default 0. 0. 0. 0. 0. 0. 1.)"
            "            ->"
            "            EC(?entity ?default)]\n",
            core.reasoner().net()
        );

        auto entity = sempr::Entity::create();
        auto geom = std::make_shared<sempr::GeosGeometry>();
        entity->addComponent(geom);

        core.addEntity(entity);
        core.performInference();
        auto wmes = core.reasoner().getCurrentState().getWMEs();

        // there should be 2: One for <Entity, GeosGeometry>, one for
        // <Entity, AffineTransform>
        BOOST_CHECK(wmes.size() == 2);

        bool affine_found = false;
        for (auto wme : wmes)
        {
            auto ec = std::dynamic_pointer_cast<sempr::ECWME>(wme);
            if (ec)
            {
                auto comp = std::get<1>(ec->value_);
                auto tf = std::dynamic_pointer_cast<sempr::AffineTransform>(comp);
                affine_found = true;
                break;
            }
        }
        BOOST_CHECK(affine_found);
    }

    BOOST_AUTO_TEST_CASE(infer_with_tag)
    {
        sempr::Core core;
        auto& parser = core.parser();
        parser.registerNodeBuilder<sempr::ECNodeBuilder<sempr::AffineTransform>>();
        parser.registerNodeBuilder<sempr::ECNodeBuilder<sempr::GeosGeometry>>();
        parser.registerNodeBuilder<sempr::AffineTransformCreateBuilder>();
        parser.registerNodeBuilder<sempr::InferECBuilder>();

        auto rules = core.addRules(
            "[defaultTF: EC<GeosGeom>(?entity ?geom),"
            "            tf:create(?default 0. 0. 0. 0. 0. 0. 1.)"
            "            ->"
            "            EC(?entity ?default \"default_tf\")]\n"
            "[test: EC<Transform>(?e ?add \"default_tf\")"
            " -> (<test> <is> <success>)]"
        );

        auto entity = sempr::Entity::create();
        auto geom = std::make_shared<sempr::GeosGeometry>();
        entity->addComponent(geom);
        core.addEntity(entity);

        core.performInference();
        std::ofstream("infer_with_tag.dot") << core.reasoner().net().toDot();

        auto wmes = core.reasoner().getCurrentState().getWMEs();
        BOOST_CHECK(contains(wmes, "<test>", "<is>", "<success>"));
    }

BOOST_AUTO_TEST_SUITE_END()
