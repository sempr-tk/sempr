#define BOOST_TEST_MODULE "C++ Unit Tests for SEMPR"
#include <boost/test/included/unit_test.hpp>

#include "Core.hpp"
#include "Entity.hpp"
#include "Exception.hpp"
#include "SimpleIncrementalIDGenerator.hpp"
#include "SeparateFileStorage.hpp"

#include "component/AffineTransform.hpp"
#include "component/GeosGeometry.hpp"
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

        // TripleVector
        auto tv = std::make_shared<sempr::TripleVector>();
        tv->addTriple({"<foo>", "<bar>", "<baz>"});
        tv->addTriple({"<foobar>", "<barbaz>", "<foobarbaz>"});
        entity->addComponent(tv);
    }

BOOST_AUTO_TEST_SUITE_END()
