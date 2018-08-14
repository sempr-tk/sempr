#include <geos/geom/GeometryFactory.h>
#include "test_utils.hpp"

using namespace testing;

BOOST_AUTO_TEST_SUITE(pointcloud)
    const std::string db_path = "test_pointcloud_sqlite.db";  //todo seg. fault after renameing

    BOOST_AUTO_TEST_CASE(simple_pointcloud)
    {
        sempr::entity::PointCloud::Ptr pc = std::make_shared<sempr::entity::PointCloud>();

        std::vector<geom::Coordinate> coords;
        std::vector<double> r;
        std::vector<double> g;
        std::vector<double> b;

        geom::Coordinate dummyPosition(1, 1, 1);
        coords.push_back(dummyPosition);

        r.push_back(1);
        g.push_back(0);
        b.push_back(0);

        pc->setCoordinates(coords);

        pc->setChannel(ChannelType::R, r);
        pc->setChannel(ChannelType::G, g);
        pc->setChannel(ChannelType::B, b);

        BOOST_CHECK(pc->hasChannel(ChannelType::G));
        BOOST_CHECK(pc->getChannel(ChannelType::G)[0] == 0);

        BOOST_CHECK((*pc)[0][0] == 1);  //check if the x 
    }

    

BOOST_AUTO_TEST_SUITE_END()
