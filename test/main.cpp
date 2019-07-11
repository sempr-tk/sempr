#include <sempr/entity/example/CoffeeMug.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
using namespace sempr::entity;



#include <sempr/query/ObjectQuery.hpp>
using namespace sempr::query;

#include <fstream>
#include <iostream>

#include <Person_odb.h>
#include <Point_odb.h>
#include <Polygon_odb.h>
#include <RuleSet_odb.h>

#include <sempr/core/IncrementalIDGeneration.hpp>
#include <sempr/entity/spatial/reference/LocalCS.hpp>
#include <chrono>

#include <RDFDocument_odb.h>

#include <sempr/sempr.hpp>
using namespace sempr::storage;

#include <rete-core/ReteCore.hpp>
#include <rete-reasoner/RuleParser.hpp>
#include <rete-reasoner/Reasoner.hpp>

#include "sempr/core/EntityAlphaBuilder.hpp"
#include "sempr/core/EntityWME.hpp"

#include <fstream>
void save(rete::Network& net, const std::string& filename)
{
    std::ofstream out(filename);
    out << net.toDot();
    out.close();
}

int main(int argc, char** args)
{
    // test the integration of entities into the reasoner -- standalone, not integrated in the core yet.
    using namespace sempr::core;

    rete::RuleParser parser;
    parser.registerNodeBuilder<EntityAlphaBuilder<Entity>>();
    parser.registerNodeBuilder<EntityAlphaBuilder<Geometry>>();
    parser.registerNodeBuilder<EntityAlphaBuilder<Polygon>>();

    rete::Reasoner reasoner;

    parser.parseRules(
        "[Entity(?e ?id), print(?id \"is an entity\") -> (<there is> <no> <result>)]\n"
        "[Geometry(?g ?id), print(?id \"is a Geometry\") -> (<there is> <no> <result>)]\n"
        "[Polygon(?p ?id), print(?id \"is a Polygon\") -> (<there is> <no> <result>)]\n"
//        "[Entity(?e ?id), Polygon(?e ?idb) -> (<this> <should> <fail>)]\n"
        ,
        reasoner.net()
    );


    // create a few entities to test the alpha conditions.
    auto e1 = std::make_shared<Entity>();
    auto e2 = std::make_shared<Entity>();
    auto g1 = std::make_shared<Geometry>();
    auto g2 = std::make_shared<Geometry>();
    auto p1 = std::make_shared<Polygon>();
    auto p2 = std::make_shared<Polygon>();

    auto assertion = std::make_shared<rete::AssertedEvidence>("fact-group-1");

    auto wme = std::make_shared<EntityWME>(e1); reasoner.addEvidence(wme, assertion);
    wme = std::make_shared<EntityWME>(e2); reasoner.addEvidence(wme, assertion);
    wme = std::make_shared<EntityWME>(g1); reasoner.addEvidence(wme, assertion);
    wme = std::make_shared<EntityWME>(g2); reasoner.addEvidence(wme, assertion);
    wme = std::make_shared<EntityWME>(p1); reasoner.addEvidence(wme, assertion);
    wme = std::make_shared<EntityWME>(p2); reasoner.addEvidence(wme, assertion);

    reasoner.performInference();

    save(reasoner.net(), "entity_rules.dot");

    reasoner.removeEvidence(std::make_shared<EntityWME>(g1), assertion);
    reasoner.removeEvidence(std::make_shared<EntityWME>(g2), assertion);

    reasoner.performInference();

    save(reasoner.net(), "entity_rules_retractedgeoms.dot");

    return 0;
}
