#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include <fstream>

#include <rete-reasoner/Reasoner.hpp>
#include <rete-reasoner/AssertedEvidence.hpp>
#include <rete-reasoner/RuleParser.hpp>
#include <rete-rdf/Triple.hpp>

#include "Core.hpp"
#include "Exception.hpp"
#include "Entity.hpp"
#include "component/TripleContainer.hpp"
#include "component/TripleVector.hpp"
#include "nodes/ECNodeBuilder.hpp"
#include "nodes/ExtractTriplesBuilder.hpp"
#include <memory>


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


BOOST_AUTO_TEST_SUITE(TripleContainerTest)
    BOOST_AUTO_TEST_CASE(addTripleContainer)
    {
        rete::Reasoner reasoner;
        rete::RuleParser parser;
        parser.registerNodeBuilder<ECNodeBuilder<TripleContainer>>();
        parser.registerNodeBuilder<ExtractTriplesBuilder>();

        parser.parseRules(
            "[extract: EC<TripleContainer>(?entity ?component) -> ExtractTriples(?component)]\n"
            "[transitive: (?a <foo> ?b), (?b <foo> ?c) -> (?a <foo> ?c)]",
            reasoner.net()
        );

        // create an entity
        auto entity = Entity::create();
        entity->setId("Entity_1"); // usually set when added to the core

        // create a TripleVector component
        auto container = std::make_shared<TripleVector>();

        // fill the component with data
        container->addTriple(Triple("<a>", "<foo>", "<b>"));
        container->addTriple(Triple("<b>", "<foo>", "<c>"));

        std::cout << "container->size(): " << container->size() << std::endl;
        for (auto t : *container)
        {
            std::cout << "triple: " <<
                t.getField(Triple::Field::SUBJECT) << " " <<
                t.getField(Triple::Field::PREDICATE) << " " <<
                t.getField(Triple::Field::OBJECT) << std::endl;
        }

        // add the component to the entity
        entity->addComponent(container);

        // add the entity-component-pair to the reasoner
        auto evidence = std::make_shared<rete::AssertedEvidence>("FactGroup-1");
        auto ecwme = std::make_shared<ECWME>(entity, container);
        reasoner.addEvidence(ecwme, evidence);

        // perfom inference and output network as dot
        reasoner.performInference();

        reasoner.printHistory();
        std::ofstream("triplecontainer.dot") << reasoner.net().toDot();

        // check if (<a> <foo> <b>) has been inferred
        auto wmes = reasoner.getCurrentState().getWMEs();
        BOOST_CHECK(contains(wmes, "<a>", "<foo>", "<b>")); // Triples not extracted

        // check if (<a> <foo> <c>) has been inferred
        BOOST_CHECK(contains(wmes, "<a>", "<foo>", "<c>")); // No reasong with extracted triples


        // -------------------
        // remove a single entry from the TripleVector component
        container->removeTripleAt(0); // removes <a> <foo> <b>
        container->changed(); // does nothing, we don't use a sempr::Core in this test
        reasoner.net().getRoot()->activate(ecwme, rete::PropagationFlag::UPDATE); // hence this activation, manually

        reasoner.performInference();
        wmes = reasoner.getCurrentState().getWMEs();

        // check if <a> <foo> <b> is gone
        BOOST_CHECK(!contains(wmes, "<a>", "<foo>", "<b>"));
        // check if inferred <a> <foo> <c> was removed, too
        BOOST_CHECK(!contains(wmes, "<a>", "<foo>", "<c>"));

        // --------------------
        // remove the entity-component-pair
        reasoner.removeEvidence(evidence);
        reasoner.performInference();

        // check if there are no more wmes
        wmes = reasoner.getCurrentState().getWMEs();
        BOOST_CHECK(wmes.empty());
    }
BOOST_AUTO_TEST_SUITE_END()

