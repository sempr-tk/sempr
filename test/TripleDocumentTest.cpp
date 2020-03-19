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
#include "nodes/ECNodeBuilder.hpp"
#include "nodes/LoadTriplesFromFileBuilder.hpp"
#include "nodes/TripleDocumentFilenameBuilder.hpp"
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


BOOST_AUTO_TEST_SUITE(TripleDocumentTest)
    BOOST_AUTO_TEST_CASE(addTripleDocument_unmonitored)
    {
        // remove the triple_document.rdf prior to testing
        std::remove("triple_document.rdf");

        sempr::Core core;

        rete::RuleParser parser;
        parser.registerNodeBuilder<ECNodeBuilder<TripleDocument>>();
        parser.registerNodeBuilder<LoadTriplesFromFileBuilder>();
        parser.registerNodeBuilder<TripleDocumentFilenameBuilder>();

        parser.parseRules(
            "[extract: EC<TripleDocument>(?entity ?component), td:filename(?filename ?component) -> LoadTriplesFromFile(?filename)]\n"
            "[transitive: (?a <example:foo> ?b), (?b <example:foo> ?c) -> (?a <example:foo> ?c)]",
            core.reasoner().net()
        );

        {
            std::ofstream("tripledocument.dot") << core.reasoner().net().toDot();
        }

        // create an entity
        auto entity = Entity::create();

        // create a TripleDocument component
        auto container = std::make_shared<TripleDocument>("./triple_document.rdf");

        // add the component to the entity
        entity->addComponent(container);

        // add some data:
        std::string data =
        R"del(
@prefix ex: <example:> .

ex:a ex:foo ex:b .
ex:b ex:foo ex:c .
        )del";

        {
            std::ofstream("triple_document.rdf") << data;
        }

        // add everything to the core
        core.addEntity(entity);

        // perfom inference and output network as dot
        core.performInference();

        {
            std::ofstream("tripledocument.dot") << core.reasoner().net().toDot();
        }

        // check if (<a> <foo> <b>) has been extracted
        auto wmes = core.reasoner().getCurrentState().getWMEs();
        BOOST_CHECK(contains(wmes, "<example:a>", "<example:foo>", "<example:b>"));

        // check if (<a> <foo> <c>) has been inferred
        BOOST_CHECK(contains(wmes, "<example:a>", "<example:foo>", "<example:c>"));
    }



    BOOST_AUTO_TEST_CASE(addTripleDocument_monitored)
    {
        /**
            This test is for the combination of the file monitor (file:exists)
            and the triple document.
        */

        // remove the triple_document.rdf prior to testing
        std::remove("triple_document.rdf");

        sempr::Core core;

        rete::RuleParser parser;
        parser.registerNodeBuilder<ECNodeBuilder<TripleDocument>>();
        parser.registerNodeBuilder<LoadTriplesFromFileBuilder>();
        parser.registerNodeBuilder<TripleDocumentFilenameBuilder>();
        parser.registerNodeBuilder<FileMonitorNodeBuilder>(core.reasonerMutex());

        parser.parseRules(
            "[extract: EC<TripleDocument>(?entity ?component), td:filename(?filename ?component), "
            "          file:exists(?filename)"
            " -> "
            "          LoadTriplesFromFile(?filename)]",
            core.reasoner().net()
        );

        {
            std::ofstream("tripledocument_monitored_0.dot") << core.reasoner().net().toDot();
        }

        // create an entity
        auto entity = Entity::create();

        // create a TripleDocument component
        auto container = std::make_shared<TripleDocument>("./triple_document.rdf");

        // add the component to the entity
        entity->addComponent(container);

        // add everything to the core
        core.addEntity(entity);

        // wait a bit for the file monitor threads to catch up
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        core.performInference();
        auto wmes = core.reasoner().getCurrentState().getWMEs();
        BOOST_CHECK(!contains(wmes, "<example:a>", "<example:foo>", "<example:b>"));
        BOOST_CHECK(wmes.size() == 1); // there should only be the TripleDocument.

        {
            std::ofstream("tripledocument_monitored_1.dot") << core.reasoner().net().toDot();
        }

        // ---------------------------
        // NOW: add some data
        // ---------------------------
        std::string data =
R"del(
@prefix ex: <example:> .
ex:a ex:foo ex:b .
)del";

        {
            std::ofstream("triple_document.rdf") << data;
        }
        // wait a bit for the file monitor threads to catch up
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // perfom inference
        core.performInference();

        // (a foo b) should be there now
        wmes = core.reasoner().getCurrentState().getWMEs();
        BOOST_CHECK(contains(wmes, "<example:a>", "<example:foo>", "<example:b>"));

        {
            std::ofstream("tripledocument_monitored_2.dot") << core.reasoner().net().toDot();
        }

        // lets *modify* the file by appending data
        {
            std::ofstream ofs;
            ofs.open("triple_document.rdf", std::ofstream::out | std::ofstream::app);
            ofs << "\n"
                << "ex:b ex:foo ex:c . \n";
            ofs.close();
        }
        // wait a bit for the file monitor threads to catch up
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        core.performInference();

        {
            std::ofstream("tripledocument_monitored_3.dot") << core.reasoner().net().toDot();
        }

        // now, (b foo c) should have been added
        wmes = core.reasoner().getCurrentState().getWMEs();
        BOOST_CHECK(contains(wmes, "<example:b>", "<example:foo>", "<example:c>"));

        // --------------------------
        // NOW: remove the file
        // --------------------------
        std::remove("triple_document.rdf");

        // wait a bit for the file monitor threads to catch up
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        core.performInference();
        wmes = core.reasoner().getCurrentState().getWMEs();

        // should be only the TripleDocument in there
        BOOST_CHECK(wmes.size() == 1);

        {
            std::ofstream("tripledocument_monitored_4.dot") << core.reasoner().net().toDot();
        }

    }

BOOST_AUTO_TEST_SUITE_END()

