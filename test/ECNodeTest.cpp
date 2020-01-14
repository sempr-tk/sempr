#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include <fstream>

#include <rete-reasoner/Reasoner.hpp>
#include <rete-reasoner/AssertedEvidence.hpp>
#include <rete-reasoner/RuleParser.hpp>
#include <rete-reasoner/Exceptions.hpp>
#include <rete-rdf/Triple.hpp>

#include "Core.hpp"
#include "Exception.hpp"
#include "Entity.hpp"
#include "nodes/ECNodeBuilder.hpp"
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


BOOST_AUTO_TEST_SUITE(ECNodeTest)
    BOOST_AUTO_TEST_CASE(ParseCorrect)
    {
        rete::Reasoner reasoner;
        rete::RuleParser parser;
        parser.registerNodeBuilder<ECNodeBuilder<Component>>();

        bool ok = parser.parseRules(
            "[rule1: EC<Component>(?e ?c) -> (<a> <b> <c>)]\n" // optional tag left out
            "[rule2: EC<Component>(?e ?c ?t) -> (<a> <b> <c>)]\n" // tag as variable
            "[rule3: EC<Component>(?e ?c \"foo\") -> (<a> <b> <c>)]", // tag as const string
            reasoner.net()
        );

        BOOST_CHECK(ok);
    }

    BOOST_AUTO_TEST_CASE(ParseFailNumArgs)
    {
        rete::Reasoner reasoner;
        rete::RuleParser parser;
        parser.registerNodeBuilder<ECNodeBuilder<Component>>();

        std::string tooFewArgs = "[rule1: EC<Component>(?e) -> (<a> <b> <c>)]";
        std::string tooManyArgs = "[rule2: EC<Component>(?e ?c ?t ?foo) -> (<a> <b> <c>)]";

        BOOST_CHECK_THROW(
            parser.parseRules(tooFewArgs, reasoner.net()),
            rete::RuleConstructionException
        );


        BOOST_CHECK_THROW(
            parser.parseRules(tooManyArgs, reasoner.net()),
            rete::RuleConstructionException
        );
    }


    BOOST_AUTO_TEST_CASE(JoinTagValue)
    {
        rete::Reasoner reasoner;
        rete::RuleParser parser;
        parser.registerNodeBuilder<ECNodeBuilder<Component>>();

        std::string joinOnTag = "[rule1: EC<Component>(?e ?c ?tag), (?a <b> ?tag) -> (<foo> <bar> <baz>)]";

        parser.parseRules(joinOnTag, reasoner.net());

        // create an entity
        auto entity = Entity::create();
        entity->setId("Entity_1"); // usually set when added to the core
        auto component = std::make_shared<Component>();
        // set the components tag!
        component->setTag("this is a tag");
        entity->addComponent(component);

        // add the pair to the reasoner, and also a triple
        auto evidence = std::make_shared<rete::AssertedEvidence>("FactGroup-1");
        auto ecwme = std::make_shared<ECWME>(entity, component);

        // this is a malformed triple: the object is not valid without quotes.
        // Actually, rdf specifies literals as:
        //      "lexical representation"^^<datatype uri>
        // But many concrete syntaxes assume string as the default datatype.
        // Still, the quotation marks are needed.
        auto triple = std::make_shared<rete::Triple>("<a>", "<b>", "this is a tag");
        // This would be better:
        //auto triple = std::make_shared<rete::Triple>("<a>", "<b>", "\"this is a tag\"");
        // But the triple accessor in the rete network only gives us the raw content,
        // including the quotation marks, not the actual value. But the tag of
        // the component does not have quotation marks...
        // What to do? ... See https://git.ni.dfki.de/sempr/rete/issues/13

        reasoner.addEvidence(ecwme, evidence);
        reasoner.addEvidence(triple, evidence);

        reasoner.performInference();
        auto wmes = reasoner.getCurrentState().getWMEs();

        BOOST_CHECK(
            contains(wmes, "<foo>", "<bar>", "<baz>")
        );

        std::ofstream("JoinTagValue.dot") << reasoner.net().toDot();
    }

    BOOST_AUTO_TEST_CASE(CheckTagValue)
    {
        rete::Reasoner reasoner;
        rete::RuleParser parser;
        parser.registerNodeBuilder<ECNodeBuilder<Component>>();

        std::string checkTagValue = "[rule1: EC<Component>(?e ?c \"this is a tag\") -> (<foo> <bar> <baz>)]";
        parser.parseRules(checkTagValue, reasoner.net());

        auto entity = Entity::create();
        entity->setId("Entity_1");
        auto component = std::make_shared<Component>();
        component->setTag("\"this is a tag\""); // NOTE: See sempr/rete issue #13
        entity->addComponent(component);
        auto evidence = std::make_shared<rete::AssertedEvidence>("FactGroup-1");
        auto ecwme = std::make_shared<ECWME>(entity, component);

        reasoner.addEvidence(ecwme, evidence);
        reasoner.performInference();
        auto wmes = reasoner.getCurrentState().getWMEs();

        std::ofstream("CheckTagValue.dot") << reasoner.net().toDot();

        BOOST_CHECK(contains(wmes, "<foo>", "<bar>", "<baz>"));
    }
BOOST_AUTO_TEST_SUITE_END()

