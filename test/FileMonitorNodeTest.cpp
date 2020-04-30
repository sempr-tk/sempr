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
#include "component/TripleContainer.hpp"
#include "component/TripleDocument.hpp"
#include "component/TripleVector.hpp"
#include "nodes/ECNodeBuilder.hpp"
#include "nodes/ExtractTriplesBuilder.hpp"
#include "nodes/FileMonitorNodeBuilder.hpp"
#include <memory>
#include <chrono>
#include <thread>


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


BOOST_AUTO_TEST_SUITE(FileMonitorNodeTest)
    BOOST_AUTO_TEST_CASE(test1)
    {
        std::string testfile = "filemonitor_testfile.txt";

        // remove the testfile
        std::remove(testfile.c_str());

        sempr::Core core;

        rete::RuleParser parser;
        parser.registerNodeBuilder<ECNodeBuilder<TripleContainer>>();
        parser.registerNodeBuilder<ExtractTriplesBuilder>();
        parser.registerNodeBuilder<FileMonitorNodeBuilder>(core.reasonerMutex());

        auto rules = parser.parseRules(
            "[extract: EC<TripleContainer>(?entity ?component) -> ExtractTriples(?component)]\n"
            "[monitor: (?f <type> <file>), (?f <path> ?p), file:exists(?p) -> (?f <exists> \"true\")]",
            core.reasoner().net()
        );

        // create an entity
        auto entity = Entity::create();

        // create a TripleVector for some basic data
        auto container = std::make_shared<TripleVector>();

        // add the component to the entity
        entity->addComponent(container);

        // add everything to the core
        core.addEntity(entity);

        // add some data:
        container->addTriple({ "<file1>", "<type>", "<file>"});
        container->addTriple({ "<file1>", "<path>", testfile});

        // trigger activations in rete network
        container->changed();

        // wait a bit for the monitor to work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::ofstream("filemonitor_beforePI.dot") << core.reasoner().net().toDot();

        // perfom inference and output network as dot
        core.performInference();

        std::ofstream("filemonitor_afterPI_0.dot") << core.reasoner().net().toDot();

        // check if (<file1> <exists> "true") has *NOT* been extracted (file does not exist yet)
        auto wmes = core.reasoner().getCurrentState().getWMEs();
        BOOST_CHECK(!contains(wmes, "<file1>", "<exists>", "\"true\""));

        // create the file.
        {
            std::ofstream(testfile) << "foo!" << std::endl;
        }

        // let the monitor run for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // perform inference to infer the triple stating that the file exists now
        // (this does not trigger a "check", it only executes already scheduled
        // effects!)
        core.performInference();
        wmes = core.reasoner().getCurrentState().getWMEs();

        std::ofstream("filemonitor_afterPI_1.dot") << core.reasoner().net().toDot();

        // should exist now.
        BOOST_CHECK(contains(wmes, "<file1>", "<exists>", "\"true\""));

        // remove the file again
        std::remove(testfile.c_str());

        // let the monitor run for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        core.performInference();
        wmes = core.reasoner().getCurrentState().getWMEs();
        std::ofstream("filemonitor_afterPI_2.dot") << core.reasoner().net().toDot();

        // should no longer exist.
        BOOST_CHECK(!contains(wmes, "<file1>", "<exists>", "\"true\""));
    }
BOOST_AUTO_TEST_SUITE_END()

