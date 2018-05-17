#include "test_utils.hpp"
#include <SpatialObject_odb.h>
using namespace testing;

BOOST_AUTO_TEST_SUITE(general_tests)

  std::string db_path = "test_sqlite.db";

  BOOST_AUTO_TEST_CASE(create_db)
  {
    {
      ODBStorage::Ptr str = setUpStorage(db_path,true);
    }
    {
      ODBStorage::Ptr str = loadStorage(db_path);
    }
    removeStorage(db_path);
  }

  BOOST_AUTO_TEST_CASE(core_test)
  {
    ODBStorage::Ptr storage = setUpStorage(db_path,true);

    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );

    Core core;
    core.addModule(active);
    core.addModule(updater);

    removeStorage(db_path);
  }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(basic_entity_test)

std::string db_path = "basic_entity_test.db";

BOOST_AUTO_TEST_CASE(insertion)
{
  // insertion
    // setup. database will be reset (true)
    ODBStorage::Ptr storage = setUpStorage(db_path, true);
    Core core;
    DBUpdateModule::Ptr updater(new DBUpdateModule(storage));
    core.addModule(updater);

    // load all entites, check if there are exactly 0 (person and its RDF)
    // checkEntitiesInStorage(storage, 0);
    // ^ sorry to disable this, but the id generation adds entities, too...

    // create  a person, which creates an rdf-entity within
    Person::Ptr p1(new Person());

    // save --> persist the person. check 1: we should not get a foreign-key-
    // constraint-violation if the rdf-entity is persisted before the person.
    core.addEntity(p1);

    // explicitly save the current state.
    updater->updateDatabase();

    // this doesn't hold anymore: the IncrementalIDGeneration adds some
    // entities, too!
    // // load all entites, check if there are exactly 2 (person and its RDF).
    // checkEntitiesInStorage(storage, 2);
    // there should be exactly 1 Person and 1 RDFPropertyMap
    checkEntitiesInStorage<Person>(storage, 1);
    checkEntitiesInStorage<RDFPropertyMap>(storage, 1);

    removeStorage(db_path);
}

BOOST_AUTO_TEST_CASE(retrieval){
    // retrieval. New ODBStorage, new session, force to load from database
  std::string id;
  std::string first = "Peter";
  std::string last = "Müller";
  Person::Gender gender = Person::Gender::MALE;

  {
    ODBStorage::Ptr storage = setUpStorage(db_path, true);
    Core core;
    DBUpdateModule::Ptr updater(new DBUpdateModule(storage));
    core.addModule(updater);

    // create  a person, which creates an rdf-entity within
    Person::Ptr person(new Person());
    // save --> persist the person. check 1: we should not get a foreign-key-
    // constraint-violation if the rdf-entity is persisted before the person.

    person->setFirst(first);
    person->setLast(last);
    person->setGender(gender);

    core.addEntity(person);
    updater->updateDatabase();

    id = person->id();
  }

  {
    //core created anew to force a new session, retrieve entity"
    ODBStorage::Ptr storage = loadStorage(db_path);
    Core core;

    DBObject::Ptr p1 = storage->load(id);
    Person::Ptr p2 = storage->load<Person>(id);

    // check if the object is of correct type by using the discriminator.
    BOOST_CHECK_EQUAL(p1->discriminator(), DISCRIMINATOR(Person));

    BOOST_CHECK_EQUAL(p2->getFirst(),first);
    BOOST_CHECK_EQUAL(p2->getLast(),last);
    BOOST_CHECK_EQUAL(p2->getGender(),gender);

    removeStorage(db_path);
  }
}

BOOST_AUTO_TEST_CASE(update) {
  //DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
  //DebugModule::Ptr debug( new DebugModule() );
  //core.addModule(debug);
}

BOOST_AUTO_TEST_CASE(deletion) {
  std::string id;


  ODBStorage::Ptr storage = setUpStorage(db_path, true);
  Core core;
  DBUpdateModule::Ptr updater(new DBUpdateModule(storage));
  core.addModule(updater);

  Person::Ptr person(new Person());
  core.addEntity(person);
  updater->updateDatabase();
  id = person->id();

  // load all entites, check if there are exactly one person and its RDF.
  checkEntitiesInStorage<Person>(storage, 1);
  checkEntitiesInStorage<RDFPropertyMap>(storage, 1);

  core.removeEntity(person);
  updater->updateDatabase();

  checkEntitiesInStorage<Person>(storage, 0);
  checkEntitiesInStorage<RDFPropertyMap>(storage, 0);

  removeStorage(db_path);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(register_children_no_duplicates)
    std::string db_path = "test_sqlite.db";

    BOOST_AUTO_TEST_CASE(register_children_no_duplicates_test)
    {
        size_t numBefore;
        std::string personId;

        {
            ODBStorage::Ptr storage = setUpStorage(db_path, true);
            Core core;
            DBUpdateModule::Ptr updater(new DBUpdateModule(storage));
            core.addModule(updater);

            Person::Ptr person(new Person());
            core.addEntity(person);
            updater->updateDatabase();
            personId = person->id();
        }

        {
            ODBStorage::Ptr storage = loadStorage(db_path);
            Core core;

            // load: creates an RDFPropertyMap in the ctor
            Person::Ptr person = storage->load<Person>(personId);

            // get the current number of RDFPropertyMaps in the database.
            std::vector<RDFPropertyMap::Ptr> all;
            storage->loadAll<RDFPropertyMap>(all);
            numBefore = all.size();

            // assure that there is exactly one RDFPropertyMap (for the one Person)
            BOOST_CHECK_EQUAL(numBefore, 1);

            // save: this should *not* add another entity.
            storage->save(person);
        }
        {
            ODBStorage::Ptr storage = loadStorage(db_path);
            Core core;

            std::vector<RDFPropertyMap::Ptr> all;
            storage->loadAll<RDFPropertyMap>(all);

            checkEntitiesInStorage<RDFPropertyMap>(storage, numBefore); // <-- should be the same as before.
        }

        removeStorage(db_path);
    }
BOOST_AUTO_TEST_SUITE_END()


/**
    Issue #20:
    Without loading all entities before creating new ones, the IncrementalIDGeneration seems to
    falsely reuse ids that were already assigned. This shouldn't happen. This is a test-case for
    this issue.
*/
BOOST_AUTO_TEST_SUITE(inc_id_without_loading)
    std::string dbfile = "test_sqlite.db";
    std::string id1, id2;
    BOOST_AUTO_TEST_CASE(inc_id_without_loading_1)
    {
        {
            ODBStorage::Ptr storage = setUpStorage(dbfile, true);
            DBUpdateModule::Ptr updater(new DBUpdateModule(storage));
            Core core;
            core.addModule(updater);

            // insert a new entity
            // Entity::Ptr entity(new Entity());
            // try a more complex entity:
            SpatialObject::Ptr entity(new SpatialObject());

            // try adding a LocalCS to it
            LocalCS::Ptr cs(new LocalCS());
            core.addEntity(cs);

            entity->geometry()->setCS(cs);
            core.addEntity(entity);


            id1 = entity->id();

            updater->updateDatabase();
        }
        // let storage etc run out of scope.

        // force idgen reset to really let storage run out of scope.
        IDGenerator::getInstance().setStrategy(
            std::unique_ptr<IDGenerationStrategy>(
                new UUIDGeneration()
            )
        );

        {
            // simply repeat everything.
            // we just add a new entity without loading the old one.
            ODBStorage::Ptr storage = loadStorage(dbfile);
            DBUpdateModule::Ptr updater(new DBUpdateModule(storage));
            Core core;
            core.addModule(updater);

            // insert a new entity
            // Entity::Ptr entity(new Entity());
            SpatialObject::Ptr entity(new SpatialObject());

            // try adding a LocalCS to it
            LocalCS::Ptr cs(new LocalCS());
            core.addEntity(cs);

            entity->geometry()->setCS(cs);
            core.addEntity(entity);

            id2 = entity->id();
            BOOST_CHECK(id1 != id2);

            std::cout << id1 << " != " << id2 << std::endl;

            updater->updateDatabase();
        }
    }

    BOOST_AUTO_TEST_CASE(inc_id_without_loading_cleanup)
    {
        removeStorage(dbfile);
    }
BOOST_AUTO_TEST_SUITE_END()
