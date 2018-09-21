#include "test_utils.hpp"
using namespace testing;


geom::MultiPoint* setupQuadrangle(const std::array<float, 3>& min, const std::array<float, 3>& max)
{
    std::vector<geom::Coordinate> corners;

    corners.push_back(geom::Coordinate(min[0], min[1], min[2]));
    corners.push_back(geom::Coordinate(min[0], min[1], max[2]));
    corners.push_back(geom::Coordinate(min[0], max[1], min[2]));
    corners.push_back(geom::Coordinate(min[0], max[1], max[2]));
    corners.push_back(geom::Coordinate(max[0], min[1], min[2]));
    corners.push_back(geom::Coordinate(max[0], min[1], max[2]));
    corners.push_back(geom::Coordinate(max[0], max[1], min[2]));
    corners.push_back(geom::Coordinate(max[0], max[1], max[2]));

    auto mp = geom::GeometryFactory::getDefaultInstance()->createMultiPoint(corners);

    return mp;
}


BOOST_AUTO_TEST_SUITE(spatial_index)
    std::string dbfile = "test_spatial_index_sqlite.db";

    BOOST_AUTO_TEST_CASE(spatial_index_simple)
    {
        Core core;
        
        LocalCS::Ptr cs(new LocalCS());

        SpatialIndex<3>::Ptr index(new SpatialIndex<3>(cs));
        core.addModule(index);

        

        //build up a quadrangle
        MultiPoint::Ptr mp( new MultiPoint() );
        mp->setGeometry(setupQuadrangle({{1, 1, 1}}, {{10, 10, 10}}));
        mp->setCS(cs);
        core.addEntity(mp);

        auto queryWithinBox = SpatialIndexQuery<3>::withinBox(Eigen::Vector3d{0, 0, 0}, Eigen::Vector3d{10, 10 ,10}, cs);
        core.answerQuery(queryWithinBox);
        BOOST_CHECK_EQUAL(queryWithinBox->results.size(), 1);

        auto queryIntersecBox = SpatialIndexQuery<3>::intersectsBox(Eigen::Vector3d{1, 1, 1}, Eigen::Vector3d{2, 2 ,2}, cs);
        core.answerQuery(queryIntersecBox);
        BOOST_CHECK_EQUAL(queryIntersecBox->results.size(), 1);

    }

    BOOST_AUTO_TEST_CASE(spatial_index_complex)
    {
        Core core;

        // add a spatial refernce
        LocalCS::Ptr cs(new LocalCS());
        core.addEntity(cs);
        
        SpatialIndex<3>::Ptr index(new SpatialIndex<3>(cs));
        core.addModule(index);

        // add a few geometries, all with y/z from 0 to 1, but with different x:
        // 0  1  2  3  4  5  6  7  8  9 10
        // |p0|p1|p2|p3|p4|p5|p6|p7|p8|p9|
        for (int i = 0; i < 10; i++)
        {
            MultiPoint::Ptr mp( new MultiPoint(new PredefinedID("mp" + std::to_string(i))) );
            mp->setGeometry(setupQuadrangle({{float(i), 0, 0}}, {{float(i+1), 1, 1}}));
            mp->setCS(cs);
            core.addEntity(mp);
        }

        // add more geometries, with different coordinate systems!
        LocalCS::Ptr previous = cs;
        for (int i = 10; i < 20; i++)
        {
            MultiPoint::Ptr mp( new MultiPoint(new PredefinedID("mp" + std::to_string(i))) );
            mp->setGeometry(setupQuadrangle({{float(9), 0, 0}}, {{float(10), 1, 1}}));

            LocalCS::Ptr child(new LocalCS());  // with a new coordinate system
            child->setParent(previous);         // attached to the previous
            child->setTranslation(1, 0, 0);     // with a fixed offset of x=1
            core.addEntity(child);
            previous = child;

            mp->setCS(child);
            core.addEntity(mp);
        }

        /** now we have 20 polygons side by side along the x axis, with the first 10 in the same
            coordinate system, the last 10 chained with a const offset of 1.
            0 ----------7  8  9 10  11  12  13 -------- 20
            |p0|        |p7|p8|p9|p10|p11|p12|
            let's query for everything
                within [7.5, 12.5] --> expected 8,9,10,11
                intersects ^^ --> expected 7,8,9,10,11,12
        */
        std::set<std::string> expected_within = {{ "mp8", "mp9", "mp10", "mp11" }};
        std::set<std::string> expected_intersects = {{ "mp7", "mp8", "mp9", "mp10", "mp11", "mp12" }};

        // within
        auto query = SpatialIndexQuery<3>::withinBox(Eigen::Vector3d{7.5, -1, -1}, Eigen::Vector3d{12.5, 2, 2}, cs);
        core.answerQuery(query);

        BOOST_CHECK_EQUAL(query->results.size(), expected_within.size());
        for (auto r : query->results)
        {
            BOOST_CHECK( expected_within.find(r->id()) != expected_within.end() );
        }

        // intersects
        query->results.clear();
        query->mode(SpatialQueryType::INTERSECTS);
        core.answerQuery(query);

        BOOST_CHECK_EQUAL(query->results.size(), expected_intersects.size());
        for (auto r : query->results)
        {
            BOOST_CHECK( expected_intersects.find(r->id()) != expected_intersects.end() );
        }
    }

    BOOST_AUTO_TEST_CASE(spatial_index_cleanup)
    {
        removeStorage(dbfile);
    }

BOOST_AUTO_TEST_SUITE_END()
