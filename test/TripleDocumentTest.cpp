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


BOOST_AUTO_TEST_SUITE(TripleDocumentTest)
    BOOST_AUTO_TEST_CASE(addTripleDocument)
    {
        // remove the triple_document.rdf prior to testing, to force an
        // exception when trying to load it:
        std::remove("triple_document.rdf");

        sempr::Core core;

        rete::RuleParser parser;
        parser.registerNodeBuilder<ECNodeBuilder<TripleContainer>>();
        parser.registerNodeBuilder<ExtractTriplesBuilder>();

        parser.parseRules(
            "[extract: EC<TripleContainer>(?entity ?component) -> ExtractTriples(?component)]\n"
            "[transitive: (?a <example:foo> ?b), (?b <example:foo> ?c) -> (?a <example:foo> ?c)]",
            core.reasoner().net()
        );

        // create an entity
        auto entity = Entity::create();

        // create a TripleDocument component
        auto container = std::make_shared<TripleDocument>("triple_document.rdf");
        BOOST_CHECK_THROW(container->reload(), sempr::Exception); // the file does not exist yet

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

        // reload!
        container->reload();

        // add everything to the core
        core.addEntity(entity);

        // perfom inference and output network as dot
        core.reasoner().performInference();

        std::ofstream("tripledocument.dot") << core.reasoner().net().toDot();

        // check if (<a> <foo> <b>) has been extracted
        auto wmes = core.reasoner().getCurrentState().getWMEs();
        BOOST_CHECK(contains(wmes, "<example:a>", "<example:foo>", "<example:b>"));

        // check if (<a> <foo> <c>) has been inferred
        BOOST_CHECK(contains(wmes, "<example:a>", "<example:foo>", "<example:c>"));
    }
BOOST_AUTO_TEST_SUITE_END()

