#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"
#include "component/GeosGeometry.hpp"
#include "nodes/GeoConversionBuilders.hpp"
#include "nodes/ECNodeBuilder.hpp"
#include "nodes/CreateEntityBuilder.hpp"
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

BOOST_AUTO_TEST_SUITE(CreateEntityTest)

    BOOST_AUTO_TEST_CASE(infer)
    {
        sempr::Core core;

        rete::RuleParser& parser = core.parser();
        parser.registerNodeBuilder<sempr::UTMFromWGSBuilder>();
        parser.registerNodeBuilder<sempr::ECNodeBuilder<sempr::GeosGeometryInterface>>();
        parser.registerNodeBuilder<sempr::CreateEntityBuilder>();
        parser.registerNodeBuilder<sempr::InferECBuilder>();


        auto rules = core.addRules(
            "[WGStoUTM:"
            "   EC<GeosGeomI>(?entity ?geom),"
            "   noValue { (?other sempr:hasUTMGeo ?entity) },"
            "   geo:UTMFromWGS(?utmGeo ?geom 32),"
            "   makeSkolem(?id ?entity \"UTM\"),"
            "   createEntity(?utmEntity ?id)"
            "   ->"
            "   (?entity sempr:hasUTMGeo ?utmEntity),"
            "   EC(?utmEntity ?utmGeo)]\n"

            "[test:"
            "   (?e sempr:hasUTMGeo ?utmEntity),"
            "   EC<GeosGeomI>(?utmEntity ?utmGeo)"
            "   ->"
            "   (<test> <is> <success>)]"
        );

        auto entity = sempr::Entity::create();
        auto geom = std::make_shared<sempr::GeosGeometry>();
        entity->addComponent(geom);

        core.addEntity(entity);
        core.performInference();
        auto wmes = core.reasoner().getCurrentState().getWMEs();

        std::ofstream("createEntity.dot") << core.reasoner().net().toDot();

        // there should be 4:
        // - <Entity, GeosGeometry>
        // - (__ hasUTMGeo __)
        // - <Entity, GeosGeometry>
        // - (<test> <is> <success>)
        BOOST_CHECK(wmes.size() == 4);
        BOOST_CHECK(contains(wmes, "<test>", "<is>", "<success>"));
    }
BOOST_AUTO_TEST_SUITE_END()
