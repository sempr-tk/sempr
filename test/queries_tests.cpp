#include "test_utils.hpp"
using namespace testing;

BOOST_AUTO_TEST_SUITE(queries)
    std::string dbfile = "test_queries_sqlite.db";
    BOOST_AUTO_TEST_CASE(SPARQLQuery_test)
    {
        ODBStorage::Ptr storage = setUpStorage(dbfile, true);
        Core core;

        ActiveObjectStore::Ptr active(new ActiveObjectStore());
        SopranoModule::Ptr semantic(new SopranoModule());
        core.addModule(active);
        core.addModule(semantic);

        // query for all persons, there should be none
        {
            SPARQLQuery::Ptr query(new SPARQLQuery());
            query->query = "SELECT ?p WHERE { ?p rdf:type sempr:Person . }";
            core.answerQuery(query);
            BOOST_CHECK_EQUAL(query->results.size(), 0);
        }

        // insert a few persons
        {
            for (int i = 10; i < 20; i++) {
                Person::Ptr p(new Person());
                p->age(i);
                p->height(1.5 + 0.01*i);
                core.addEntity(p);
            }
        }

        // query for persons of age 18 or older and get their heights
        {
            SPARQLQuery::Ptr query(new SPARQLQuery());
            // query->query = "SELECT * WHERE { ?s ?p ?o .}";
            query->query = "SELECT * WHERE { " \
                            "?p rdf:type sempr:Person." \
                            "?p sempr:age ?age ." \
                            "FILTER(?age >= 18) ." \
                            "?p sempr:height ?height ." \
                            "}";
            core.answerQuery(query);

            // there should be 2 results, aged 18 and 19
            BOOST_CHECK_EQUAL(query->results.size(), 2);
            // print their heights. just as an example usage of the query
            for (auto r : query->results) {
                // std::cout << "Query Result: " << r["p"] << " of age " << r["age"] << " is " << r["height"] << " m high." << '\n';
                for (auto b : r) {
                    std::cout << b.first << "=" << b.second << "  |  ";
                }
                std::cout << std::endl;
            }
        }

    }
    BOOST_AUTO_TEST_CASE(SPARQLQuery_test_cleanup)
    {
        removeStorage(dbfile);
    }


    BOOST_AUTO_TEST_CASE(SPARQLQuery_test_inference)
    {
        ODBStorage::Ptr storage = setUpStorage(dbfile, true);
        Core core;

        ActiveObjectStore::Ptr active(new ActiveObjectStore());
        SopranoModule::Ptr semantic(new SopranoModule());
        core.addModule(active);
        core.addModule(semantic);

        // query anything, there should be nothing
        {
            SPARQLQuery::Ptr query(new SPARQLQuery());
            query->query = "SELECT ?s WHERE { ?s ?p ?o . }";
            core.answerQuery(query);
            BOOST_CHECK_EQUAL(query->results.size(), 0);
        }

        // insert a few persons
        {
            for (int i = 0; i < 10; i++) {
                Person::Ptr p(new Person());
                p->age(i+10);
                core.addEntity(p);
            }
        }

        // query for humans, there should be none
        {
            SPARQLQuery::Ptr query(new SPARQLQuery());
            query->query = "SELECT * WHERE { " \
                            "?p rdf:type sempr:Human." \
                            "}";
            core.answerQuery(query);

            BOOST_CHECK_EQUAL(query->results.size(), 0);
        }

        // add a rule that persons are humans
        {
            RuleSet::Ptr rules(new RuleSet());
            rules->add("[personsAreHumans: (?p rdf:type sempr:Person) -> (?p rdf:type sempr:Human)]");
            core.addEntity(rules);
        }

        // query for humans, there should be 10 (as there are persons)
        {
            SPARQLQuery::Ptr query(new SPARQLQuery());
            query->query = "SELECT * WHERE { "\
                            "?p rdf:type sempr:Human." \
                            "}";
            core.answerQuery(query);
            BOOST_CHECK_EQUAL(query->results.size(), 10);
        }
    }
    BOOST_AUTO_TEST_CASE(SPARQLQuery_test_inference_cleanup)
    {
        removeStorage(dbfile);
    }



    BOOST_AUTO_TEST_CASE(ObjectQuery_test)
    {
        ODBStorage::Ptr storage = setUpStorage(dbfile, true);
        Core core;

        ActiveObjectStore::Ptr active(new ActiveObjectStore());
        core.addModule(active);

        // query for persons
        {
            auto q = std::make_shared<ObjectQuery<Person> >();
            core.answerQuery(q);
            BOOST_CHECK_EQUAL(q->results.size(), 0);
        }

        // add some stuff
        int numPersons = 10;
        for (int i = 0; i < numPersons; i++) {
            Person::Ptr p(new Person());
            p->age(i);
            core.addEntity(p);
        }

        // query.
        // query for persons
        Person::Ptr e;
        {
            auto q = std::make_shared<ObjectQuery<Person> >();
            core.answerQuery(q);
            BOOST_CHECK_EQUAL(q->results.size(), numPersons);
            e = q->results[0];
        }
        // remove someone.
        core.removeEntity(e);
        // query again
        {
            auto q = std::make_shared<ObjectQuery<Person> >();
            core.answerQuery(q);
            BOOST_CHECK_EQUAL(q->results.size(), numPersons-1);
        }

        // test the decision-function-mechanism
        {
            auto q = std::make_shared<ObjectQuery<Person> >(
                [](Person::Ptr person) { return person->age() >= 5; }
            );
            core.answerQuery(q);
            BOOST_CHECK_EQUAL(q->results.size(), 5); // aged 5, 6, 7, 8, 9
        }

        removeStorage(dbfile);
    }

BOOST_AUTO_TEST_SUITE_END()
