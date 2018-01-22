#define BOOST_TEST_MODULE sempr-test
#include <boost/test/included/unit_test.hpp>

#include <odb/core.hxx>
#include <sempr/sempr.hpp>
#include <sempr/entity/Person.hpp>
#include <Person_odb.h>
#include <iostream>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include <sempr/entity/RDFPropertyMap.hpp>

#include <sempr/query/ObjectQuery.hpp>
#include <sempr/core/IncrementalIDGeneration.hpp>

// geometries
#include <Geometry_odb.h>
#include <Point_odb.h>
#include <LineString_odb.h>
#include <Polygon_odb.h>


using namespace sempr::core;
using namespace sempr::storage;
using namespace sempr::processing;
using namespace sempr::entity;
using namespace sempr::query;

#define DISCRIMINATOR(T) (odb::object_traits_impl<T, odb::id_common>::info.discriminator)


ODBStorage::Ptr setUpStorage(std::string db_path, bool reset){

  BOOST_CHECK(!boost::filesystem::exists(db_path) );
  ODBStorage::Ptr storage( new ODBStorage(db_path, reset) );
  // set the strategy to IncrementalIDGeneration.
  IDGenerator::getInstance().setStrategy(
      std::unique_ptr<IDGenerationStrategy>(
          new IncrementalIDGeneration(storage)
      )
  );

  BOOST_CHECK(boost::filesystem::exists(db_path) );

  return storage;
}

ODBStorage::Ptr loadStorage(std::string db_path){

  BOOST_CHECK(boost::filesystem::exists(db_path));

  // need to reset the IDGeneration first: It keeps a shared_ptr to storage,
  // and storage keeps a session. If we didn't do this, we'd get
  // "std::exception: session already in effect in this thread"
  IDGenerator::getInstance().reset();

  ODBStorage::Ptr storage( new ODBStorage(db_path, false) );
  // set the strategy to IncrementalIDGeneration.
  IDGenerator::getInstance().setStrategy(
      std::unique_ptr<IDGenerationStrategy>(
          new IncrementalIDGeneration(storage)
      )
  );

  return storage;
}

void removeStorage(std::string db_path){
  IDGenerator::getInstance().reset(); // free the current strategy
  // (which keeps a pointer to ODBStorage)

  boost::filesystem::remove(db_path);
  BOOST_CHECK(!boost::filesystem::exists(db_path) );
}

void checkEntitiesInStorage(ODBStorage::Ptr storage, int n){
  std::vector<DBObject::Ptr> all;
  storage->loadAll(all);
  BOOST_CHECK_EQUAL(all.size(), n);
}

template <class Entity>
void checkEntitiesInStorage(ODBStorage::Ptr storage, int n)
{
    std::vector<std::shared_ptr<Entity> > all;
    storage->loadAll<Entity>(all);
    BOOST_CHECK_EQUAL(all.size(), n);
}


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

    Core core(storage);
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
    Core core(storage);

    // load all entites, check if there are exactly 0 (person and its RDF)
    // checkEntitiesInStorage(storage, 0);
    // ^ sorry to disable this, but the id generation adds entities, too...

    // create  a person, which creates an rdf-entity within
    Person::Ptr p1(new Person());

    // save --> persist the person. check 1: we should not get a foreign-key-
    // constraint-violation if the rdf-entity is persisted before the person.
    core.addEntity(p1);

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
    Core core(storage);

    // create  a person, which creates an rdf-entity within
    Person::Ptr person(new Person());
    // save --> persist the person. check 1: we should not get a foreign-key-
    // constraint-violation if the rdf-entity is persisted before the person.

    person->setFirst(first);
    person->setLast(last);
    person->setGender(gender);

    core.addEntity(person);

    id = person->id();
  }

  {
    //core created anew to force a new session, retrieve entity"
    ODBStorage::Ptr storage = loadStorage(db_path);
    Core core(storage);

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
  Core core(storage);
  Person::Ptr person(new Person());
  core.addEntity(person);
  id = person->id();

  // load all entites, check if there are exactly one person and its RDF.
  checkEntitiesInStorage<Person>(storage, 1);
  checkEntitiesInStorage<RDFPropertyMap>(storage, 1);

  core.removeEntity(person);

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
            Core core(storage);
            Person::Ptr person(new Person());
            core.addEntity(person);
            personId = person->id();
        }

        {
            ODBStorage::Ptr storage = loadStorage(db_path);
            Core core(storage);

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
            Core core(storage);

            std::vector<RDFPropertyMap::Ptr> all;
            storage->loadAll<RDFPropertyMap>(all);

            checkEntitiesInStorage<RDFPropertyMap>(storage, numBefore); // <-- should be the same as before.
        }

        removeStorage(db_path);
    }
BOOST_AUTO_TEST_SUITE_END()


/**
    a series of tests to check the functionality of the RDFPropertyMap
    which is used for easy storage of primitive datatypes and object-pointers
    in an RDFEntity -- and therefore useable for inference.
*/
BOOST_AUTO_TEST_SUITE(entity_RDFPropertyMap)

    // to keep between tests.
    std::string mapId, personId;
    std::string databaseFile = "test_sqlite.db";

    BOOST_AUTO_TEST_CASE(propertymap_insertion)
    {
        ODBStorage::Ptr storage = setUpStorage(databaseFile, true);
        DBUpdateModule::Ptr updater(new DBUpdateModule(storage));

        Core core(storage);
        core.addModule(updater);

        // create an entity and assign different values
        RDFPropertyMap::Ptr map(new RDFPropertyMap("subject", "http://baseURI/"));
        core.addEntity(map);
        mapId = map->id();


        RDFPropertyMap& m = *map;
        m["int"] = 42;
        m["float"] = 1.234f;
        m["string"] = "Hello, World!";

        // create another entity and point to it.
        Person::Ptr person(new Person());
        core.addEntity(person);
        personId = person->id();

        m["person"] = person;
    }


    BOOST_AUTO_TEST_CASE(propertymap_retrieval)
    {
        // load the values that were stored in the previous test and check for
        // equality. also, test some conversions.
        ODBStorage::Ptr storage = loadStorage(databaseFile);
        Core core(storage);
        DBObject::Ptr tmp = storage->load(mapId);

        // cast should succeed.
        auto map = std::dynamic_pointer_cast<RDFPropertyMap>(tmp);
        BOOST_CHECK(map);

        // check values:
        RDFPropertyMap& m = *map;
        int i = m["int"];
        BOOST_CHECK_EQUAL(i, 42);
        float f = m["float"];
        BOOST_CHECK_CLOSE(f, 1.234f, 0.00001);
        std::string s = m["string"];
        BOOST_CHECK_EQUAL(s, "Hello, World!");
        Person::Ptr person = m["person"];
        BOOST_CHECK_EQUAL(personId, person->id());

        // test invalid accesses
        BOOST_CHECK_THROW(
            RDFPropertyMap::Ptr foo = m["person"], // person != RDFPropertyMap
            std::runtime_error
        );
        BOOST_CHECK_THROW(
            Entity::Ptr foo = m["no-entity-here"],
            std::runtime_error
        );

        removeStorage(databaseFile);
    }
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(queries)
    std::string dbfile = "test_sqlite.db";
    BOOST_AUTO_TEST_CASE(ObjectQuery_test)
    {
        ODBStorage::Ptr storage = setUpStorage(dbfile, true);
        Core core(storage);

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

        removeStorage(dbfile);
    }

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(geometries)
    std::string dbfile = "test_sqlite.db";
    BOOST_AUTO_TEST_CASE(geometries_insertion)
    {
        ODBStorage::Ptr storage = setUpStorage(dbfile, true);
        Core core(storage);

        ActiveObjectStore::Ptr active(new ActiveObjectStore());
        core.addModule(active);
        DBUpdateModule::Ptr updater(new DBUpdateModule(storage));
        core.addModule(updater);

        // create a bunch of geometries
        Point::Ptr point(new Point());
        LineString::Ptr linestring(new LineString());
        Polygon::Ptr polygon(new Polygon());

        //           B
        //         /   \ .
        //  ls ---/- m  \ .
        //      A ------ C
        point->geometry()->setX(0); point->geometry()->setY(0);

        auto ring = (OGRLinearRing*) OGRGeometryFactory::createGeometry(wkbLinearRing);
        ring->addPoint(-1, -1);
        ring->addPoint(0, 1);
        ring->addPoint(1, -1);
        ring->closeRings();
        
        polygon->geometry()->addRingDirectly(ring);

        linestring->geometry()->addPoint(-5, 0);
        linestring->geometry()->addPoint(0, 0);

        // insert.
        core.addEntity(point);
        core.addEntity(linestring);
        core.addEntity(polygon);
    }

    BOOST_AUTO_TEST_CASE(geometries_operations)
    {
        ODBStorage::Ptr storage = loadStorage(dbfile);

        std::vector<Point::Ptr> points;
        std::vector<LineString::Ptr> lstrings;
        std::vector<Polygon::Ptr> polygons;

        storage->loadAll(points);
        storage->loadAll(lstrings);
        storage->loadAll(polygons);

        BOOST_CHECK_EQUAL(points.size(), 1);
        BOOST_CHECK_EQUAL(lstrings.size(), 1);
        BOOST_CHECK_EQUAL(polygons.size(), 1);

        BOOST_CHECK(points[0]->geometry()->Within(polygons[0]->geometry()));
        BOOST_CHECK(lstrings[0]->geometry()->Intersects(polygons[0]->geometry()));
    }

    BOOST_AUTO_TEST_CASE(geometries_cleanup)
    {
        removeStorage(dbfile);
    }

BOOST_AUTO_TEST_SUITE_END()
