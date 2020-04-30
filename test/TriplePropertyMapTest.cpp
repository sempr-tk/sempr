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
#include "RDF.hpp"
#include "component/TripleContainer.hpp"
#include "component/TriplePropertyMap.hpp"
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


BOOST_AUTO_TEST_SUITE(TriplePropertyMapTest)
    BOOST_AUTO_TEST_CASE(addTriplePropertyMap)
    {
        rete::Reasoner reasoner;
        rete::RuleParser parser;
        parser.registerNodeBuilder<ECNodeBuilder<TripleContainer>>();
        parser.registerNodeBuilder<ExtractTriplesBuilder>();

        auto rules = parser.parseRules(
            "[extract: EC<TripleContainer>(?entity ?component) -> ExtractTriples(?component)]\n",
            reasoner.net()
        );

        // create an entity
        auto entity = Entity::create();
        entity->setId("Entity_1"); // usually set when added to the core

        // create a TripleVector component
        auto container = std::make_shared<TriplePropertyMap>();

        // fill the component with data
        container->map_["sempr:theAnswer"] = 42;
        container->map_["sempr:pi"] = 3.1415f;
        container->map_["sempr:name"] = "john smith";
        container->map_["sempr:resource"] = { "sempr:something", true };

        // test retrieval of data
        int i = container->map_["sempr:theAnswer"];
        BOOST_CHECK(i == 42);
        BOOST_CHECK_THROW(i = container->map_["sempr:pi"], sempr::Exception);
        std::string name = container->map_["sempr:name"];
        BOOST_CHECK(name == "john smith");
        std::string type = container->map_["sempr:resource"];
        BOOST_CHECK(type == "sempr:something");

        // ---
        std::cout << "Contents before adding it to an entity" << std::endl;
        for (auto t : *container)
        {
            std::cout << "triple: " <<
                t.getField(Triple::Field::SUBJECT) << " " <<
                t.getField(Triple::Field::PREDICATE) << " " <<
                t.getField(Triple::Field::OBJECT) << std::endl;
        }

        // add the component to the entity
        entity->addComponent(container);

        // --
        std::cout << "Conents after adding it to an entity" << std::endl;
        for (auto t : *container)
        {
            std::cout << "triple: " <<
                t.getField(Triple::Field::SUBJECT) << " " <<
                t.getField(Triple::Field::PREDICATE) << " " <<
                t.getField(Triple::Field::OBJECT) << std::endl;
        }

        // add the entity-component-pair to the reasoner
        auto evidence = std::make_shared<rete::AssertedEvidence>("FactGroup-1");
        auto ecwme = std::make_shared<ECWME>(entity, container);
        reasoner.addEvidence(ecwme, evidence);

        // perfom inference
        reasoner.performInference();

        // check if triples were extracted correctly
        auto wmes = reasoner.getCurrentState().getWMEs();
        std::string subject = "<" + sempr::baseURI() + entity->id() + ">";
        BOOST_CHECK(contains(wmes, subject, "<sempr:theAnswer>", std::to_string(42)));
        BOOST_CHECK(contains(wmes, subject, "<sempr:pi>", std::to_string(3.1415f)));
        BOOST_CHECK(contains(wmes, subject, "<sempr:name>", "\"john smith\""));
        BOOST_CHECK(contains(wmes, subject, "<sempr:resource>", "<sempr:something>"));


        // -------------------
        // remove a single entry from the TripleVector component
        container->map_.erase("sempr:name"); // erase (id sempr:name "john smith")
        container->changed(); // does nothing, we don't use a sempr::Core in this test
        reasoner.net().getRoot()->activate(ecwme, rete::PropagationFlag::UPDATE); // hence this activation, manually

        reasoner.performInference();
        wmes = reasoner.getCurrentState().getWMEs();

        // check if the triple is gone
        BOOST_CHECK(!contains(wmes, subject, "<sempr:name>", "\"john smith\""));

        // export the rete state
        std::ofstream("triplepropertymap.dot") << reasoner.net().toDot();

        // --------------------
        // remove the entity-component-pair
        reasoner.removeEvidence(evidence);
        reasoner.performInference();

        // check if there are no more wmes
        wmes = reasoner.getCurrentState().getWMEs();
        BOOST_CHECK(wmes.empty());
    }
BOOST_AUTO_TEST_SUITE_END()

