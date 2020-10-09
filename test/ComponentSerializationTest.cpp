#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"
#include "SimpleIncrementalIDGenerator.hpp"
#include "SeparateFileStorage.hpp"

#include "component/AffineTransform.hpp"
#include "component/GeosGeometry.hpp"
#include "component/TripleDocument.hpp"
#include "component/TriplePropertyMap.hpp"
#include "component/TripleVector.hpp"

#include <boost/filesystem.hpp>
#include <iostream>

namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(FileStorageTest)

    BOOST_AUTO_TEST_CASE(persist)
    {
        fs::path folder("./db");
        if (!fs::exists(folder)) fs::create_directories(folder);
        auto store = std::make_shared<sempr::SeparateFileStorage>(folder.string());

        // create the core (use store for storage AND idgen!, though we don't test idgen here).
        sempr::Core core(store, store);

        // create an entity
        auto entity = sempr::Entity::create();

        // add to core -> persist
        core.addEntity(entity);

        // --------------------------------------------------------------------
        // add all the different components to make sure they are serializable
        // --------------------------------------------------------------------

        // AffineTransform
        Eigen::Affine3d a = Eigen::Affine3d::Identity();
        a.translation() = Eigen::Vector3d(1.1, 2.2, 3.3);
        a.linear() = Eigen::Quaterniond(0.5, 0.5, -0.5, -0.5).toRotationMatrix();
        auto tf = std::make_shared<sempr::AffineTransform>(a);
        entity->addComponent(tf); // already triggers persistence etc.

        // GeosGeometry
        auto geom = std::make_shared<sempr::GeosGeometry>(); // an empty one
        entity->addComponent(geom);

        // TripleDocument
        auto td = std::make_shared<sempr::TripleDocument>("/foo/bar/baz.rdf");
        entity->addComponent(td);

        // TriplePropertyMap
        auto prop = std::make_shared<sempr::TriplePropertyMap>();
        prop->map_["type"] = { "sempr:Something", true };
        prop->map_["name"] = "John Smith";
        prop->map_["age"] = 42;
        entity->addComponent(prop);

        // TripleVector
        auto tv = std::make_shared<sempr::TripleVector>();
        tv->addTriple({"<foo>", "<bar>", "<baz>"});
        tv->addTriple({"<foobar>", "<barbaz>", "<foobarbaz>"});
        entity->addComponent(tv);

    }


    /**
        This test is used to check if the saveToJSON and loadFromJSON
        principally do what I expect them to: I.e., generate a representation
        of the component and -- this is the important part -- overwrite the
        state of an existing component.
    */
    BOOST_AUTO_TEST_CASE(plain_save_load)
    {
        auto m1 = std::make_shared<sempr::TriplePropertyMap>();
        auto m2 = std::make_shared<sempr::TriplePropertyMap>();

        m1->map_["foo"] = 42;
        m2->map_["bar"] = "hello";

        sempr::Component::Ptr c1 = m1, c2 = m2;

        // goal: overwrite m1 with the contents of m2, through base pointers.
        std::stringstream ss;
        {
            cereal::JSONOutputArchive ar(ss);
            c2->saveToJSON(ar);
        }

        cereal::JSONInputArchive ar(ss);
        c1->loadFromJSON(ar);

        BOOST_CHECK(m1->map_.find("foo") == m1->map_.end()); // foo must be gone
        BOOST_CHECK(static_cast<std::string>(m1->map_["bar"]) == "hello"); // and bar must be there now.
    }

BOOST_AUTO_TEST_SUITE_END()
