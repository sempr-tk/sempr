#include <geos/geom/GeometryFactory.h>
#include "test_utils.hpp"

using namespace testing;

BOOST_AUTO_TEST_SUITE(pointcloud)
    const std::string db_path = "test_pointcloud_sqlite.db";  //todo seg. fault after renameing

    BOOST_AUTO_TEST_CASE(simple_pointcloud)
    {
        sempr::entity::PointCloud::Ptr pc = std::make_shared<sempr::entity::PointCloud>();

        std::vector<geom::Coordinate> coords;
        std::vector<uint8_t> r;
        std::vector<uint8_t> g;
        std::vector<uint8_t> b;

        geom::Coordinate dummyPosition(1, 1, 1);
        coords.push_back(dummyPosition);

        r.push_back(255);
        g.push_back(0);
        b.push_back(0);

        pc->setCoordinates(coords);

        pc->setChannel<uint8_t>(ChannelType::R, r);
        pc->setChannel<uint8_t>(ChannelType::G, g);
        pc->setChannel<uint8_t>(ChannelType::B, b);

        BOOST_CHECK(pc->hasChannel(ChannelType::G));
        BOOST_CHECK(pc->getChannel<uint8_t>(ChannelType::G)[0] == 0);

        //check iterator
        for (auto red : pc->getChannel<uint8_t>(ChannelType::R))
        {
            BOOST_CHECK_EQUAL(red, 255);
        }

        BOOST_CHECK(pc->at(0).x == 1);
    }


    BOOST_AUTO_TEST_CASE(mixed_channel)
    {
        sempr::entity::PointCloud::Ptr pc = std::make_shared<sempr::entity::PointCloud>();

        std::vector<geom::Coordinate> coords;
        std::vector<uint8_t> r;
        std::vector<uint8_t> g;
        std::vector<uint8_t> b;
        std::vector<float> i;
        std::vector<double> v;

        geom::Coordinate dummyPosition(1, 1, 1);
        coords.push_back(dummyPosition);

        r.push_back(255);
        g.push_back(0);
        b.push_back(0);

        i.push_back(0.95);
        v.push_back(2.5);

        pc->setCoordinates(coords);

        pc->setChannel<uint8_t>(ChannelType::R, r);
        pc->setChannel<uint8_t>(ChannelType::G, g);
        pc->setChannel<uint8_t>(ChannelType::B, b);

        pc->setChannel<float>(ChannelType::I, i);
        pc->setChannel<double>(ChannelType::V, v);

        BOOST_CHECK(pc->hasChannel(ChannelType::G));
        BOOST_CHECK_EQUAL(pc->getChannel<uint8_t>(ChannelType::G)[0], 0);

        //check iterator
        for (auto red : pc->getChannel<uint8_t>(ChannelType::R))
        {
            BOOST_CHECK_EQUAL(red, 255);
        }

        BOOST_CHECK_EQUAL(pc->getChannel<double>(ChannelType::V)[0], 2.5);

        // check type missmatch
        try
        {
            float test = pc->getChannel<float>(ChannelType::V)[0];
            BOOST_CHECK(test);  //error!
        }
        catch (const boost::exception& ex)
        {
        }

        BOOST_CHECK(pc->at(0).x == 1);
    }


    BOOST_AUTO_TEST_CASE(pointcloud_insertion)
    {
        Core core;

        ActiveObjectStore::Ptr active(new ActiveObjectStore());
        core.addModule(active);

        ODBStorage::Ptr storage = setUpStorage(db_path, true);
        DBUpdateModule::Ptr updater(new DBUpdateModule(storage));
        core.addModule(updater);

        // create a bunch of geometries
        sempr::entity::PointCloud::Ptr pc = std::make_shared<sempr::entity::PointCloud>();

        std::vector<geom::Coordinate> coords;
        std::vector<double> intensity;
        for(int i = 0; i < 10000; i++)
        {
            // Random numbers: Min i ... Max 10 + 1
            double x = rand() % (10 + i) + i;
            double y = rand() % (10 + i) + i;
            double z = rand() % (10 + i) + i;
            coords.push_back(geom::Coordinate(x, y, z));

            // Intensity between 0.0 and 0.99
            double intensi = rand() % 100 / 100.0;
            intensity.push_back(intensi);
        }

        pc->setCoordinates(coords);
        pc->setChannel<double>(ChannelType::I, intensity);

        // insert 
        core.addEntity(pc);
        
    }


    BOOST_AUTO_TEST_CASE(pointcloud_load)
    {
        /*
        ODBStorage::Ptr storage = loadStorage(db_path);

        std::vector<PointCloud::Ptr> clouds;

        storage->loadAll(clouds);

        BOOST_CHECK_EQUAL(clouds.size(), 1);

        BOOST_CHECK_EQUAL(clouds[0]->size(), 10000);

        BOOST_CHECK(clouds[0]->hasChannel(ChannelType::I));

        BOOST_CHECK_EQUAL(clouds[0]->getChannel(ChannelType::I).size(), 10000);
        */
        
    }

    BOOST_AUTO_TEST_CASE(pointcloud_cleanup)
    {
        removeStorage(db_path);
    }
    

BOOST_AUTO_TEST_SUITE_END()
