#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include <rete-reasoner/RuleParser.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"
#include "nodes/SopranoNodeBuilder.hpp"
#include "nodes/ExtractTriplesBuilder.hpp"
#include "nodes/ECNodeBuilder.hpp"
#include "component/TripleVector.hpp"


BOOST_AUTO_TEST_SUITE(SopranoTest)

    BOOST_AUTO_TEST_CASE(queryTest)
    {
        sempr::Core core;
        // register node builder
        rete::RuleParser parser;
        parser.registerNodeBuilder<sempr::ECNodeBuilder<sempr::TripleContainer>>();
        parser.registerNodeBuilder<sempr::ExtractTriplesBuilder>();
        // create a soprano module to use for querying
        auto soprano = std::make_shared<sempr::SopranoModule>();
        std::unique_ptr<sempr::SopranoNodeBuilder> builder(new sempr::SopranoNodeBuilder(soprano));
        parser.registerNodeBuilder(std::move(builder));

        // create a rule to infer stuff
        auto rules = parser.parseRules(
            "[extract: EC<TripleContainer>(?e ?c) -> ExtractTriples(?c)]\n"
            "[updateSoprano: (?s ?p ?o) -> SopranoModule(?s ?p ?o)]\n"
            "[foobar: (?a <ex:foo> ?b), (?b <ex:bar> ?c) -> (?a <ex:foobar> ?c)]",
            core.reasoner().net()
        );

        std::ofstream("SopranoTest.dot") << core.reasoner().net().toDot();

        // create some data
        auto entity = sempr::Entity::create();
        core.addEntity(entity);

        auto part1 = std::make_shared<sempr::TripleVector>();
        part1->addTriple({"<ex:a1>", "<ex:foo>", "<ex:a2>"});
        part1->addTriple({"<ex:b1>", "<ex:foo>", "<ex:b2>"});
        part1->addTriple({"<ex:c1>", "<ex:foo>", "<ex:c2>"});

        auto part2 = std::make_shared<sempr::TripleVector>();
        part2->addTriple({"<ex:a2>", "<ex:bar>", "<ex:a3>"});
        part2->addTriple({"<ex:b2>", "<ex:bar>", "\"ex:b3\""});
        part2->addTriple({"<ex:c2>", "<ex:bar>", "<ex:c3>"});

        // query before adding anything. expect 0 results.
        core.reasoner().performInference();


        sempr::SPARQLQuery query;
        query.query = "SELECT * WHERE { ?a <ex:foobar> ?b . }";
        soprano->answer(query);
        BOOST_CHECK(query.results.empty());

        // add part1, still expect empty
        entity->addComponent(part1);
        core.reasoner().performInference();
        soprano->answer(query);
        BOOST_CHECK(query.results.empty());

        // add part2, expect 3 results
        entity->addComponent(part2);
        core.reasoner().performInference();
        std::ofstream("SopranoTest_1.dot") << core.reasoner().net().toDot();
        soprano->answer(query);
        std::cout << query.results.size() << std::endl;
        BOOST_CHECK(query.results.size() == 3);

        for (auto result : query.results)
        {
            for (auto binding : result)
            {
                std::cout << binding.first << " = " << binding.second.second
                          << " [" << (binding.second.first == sempr::SPARQLQuery::ValueType::RESOURCE ? "RESOURCE" :
                                        (binding.second.first == sempr::SPARQLQuery::ValueType::LITERAL ? "LITERAL" : "BLANK"))
                          << "]" << std::endl;
            }
        }

        query.results.clear();

        // remove one entry from part2, expect 2 results
        part2->removeTripleAt(2);
        BOOST_CHECK(part2->size() == 2);
        part2->changed();
        part2->changed();
        part2->changed();
        core.reasoner().performInference();

        soprano->answer(query);
        BOOST_CHECK(query.results.size() == 2);
        query.results.clear();

        // remove part1, expect 0 results
        entity->removeComponent(part1);
        core.reasoner().performInference();
        soprano->answer(query);
        BOOST_CHECK(query.results.empty());
    }

BOOST_AUTO_TEST_SUITE_END()
