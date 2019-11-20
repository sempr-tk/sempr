#include "test_utils.hpp"
using namespace testing;

#include "../include/rete-reasoner/RuleParser.hpp"
#include "../include/rete-reasoner/Reasoner.hpp"
#include "../include/rete-reasoner/AssertedEvidence.hpp"
#include "sempr/core/EntityAlphaBuilder.hpp"

using namespace sempr::core;
using namespace sempr::entity;
using namespace sempr::query;

BOOST_AUTO_TEST_SUITE(entity_mutable)

    std::string databaseFile = "test_sqlite.db";

    BOOST_AUTO_TEST_CASE(entity_mutable_change_value)
    {

        rete::RuleParser parser;
        parser.registerNodeBuilder<EntityAlphaBuilder<Entity>>();
    }

BOOST_AUTO_TEST_SUITE_END()
