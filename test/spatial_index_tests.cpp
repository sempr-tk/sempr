#include "test_utils.hpp"
using namespace testing;

/*
void setupQuadrangle(OGRPolygon* poly, const std::array<float, 3>& min, const std::array<float, 3>& max)
{
    // OGRLineString* ls = (OGRLineString*) OGRGeometryFactory::createGeometry(wkbLineString);
    OGRLinearRing* lr = (OGRLinearRing*) OGRGeometryFactory::createGeometry(wkbLinearRing);
    lr->addPoint(min[0], min[1], max[2]);
    lr->addPoint(min[0], max[1], min[2]);
    lr->addPoint(min[0], max[1], max[2]);
    lr->addPoint(min[0], min[1], min[2]);
    lr->addPoint(min[0], min[1], max[2]);
    lr->addPoint(min[0], max[1], min[2]);
    lr->addPoint(min[0], max[1], max[2]);
    lr->addPoint(max[0], min[1], min[2]);
    lr->addPoint(max[0], min[1], max[2]);
    lr->addPoint(max[0], max[1], min[2]);
    lr->addPoint(max[0], max[1], max[2]);
    lr->closeRings();
    poly->addRingDirectly(lr);
}
*/

BOOST_AUTO_TEST_SUITE(spatial_index)
    std::string dbfile = "test_sqlite.db";
    BOOST_AUTO_TEST_CASE(spatial_index_1)
    {
        ODBStorage::Ptr storage = setUpStorage(dbfile, true);
        Core core;

        SpatialIndex::Ptr index(new SpatialIndex());
        core.addModule(index);

        // add a spatial refernce
        LocalCS::Ptr cs(new LocalCS());
        core.addEntity(cs);

        // add a few geometries, all with y/z from 0 to 1, but with different x:
        // 0  1  2  3  4  5  6  7  8  9 10
        // |p0|p1|p2|p3|p4|p5|p6|p7|p8|p9|
        for (int i = 0; i < 10; i++)
        {
            Polygon::Ptr p( new Polygon(new PredefinedID("p" + std::to_string(i))) );
            //setupQuadrangle(p->geometry(), {{float(i), 0, 0}}, {{float(i+1), 1, 1}});
            p->setCS(cs);
            core.addEntity(p);
        }

        // add more geometries, with different coordinate systems!
        LocalCS::Ptr previous = cs;
        for (int i = 10; i < 20; i++)
        {
            Polygon::Ptr p( new Polygon(new PredefinedID("p" + std::to_string(i))) );
            //setupQuadrangle(p->geometry(), {{float(9), 0, 0}}, {{float(10), 1, 1}});   // always the same

            LocalCS::Ptr child(new LocalCS());  // with a new coordinate system
            child->setParent(previous);         // attached to the previous
            child->setTranslation(1, 0, 0);     // with a fixed offset of x=1
            core.addEntity(child);
            previous = child;

            p->setCS(child);
            core.addEntity(p);
        }

        /** now we have 20 polygons side by side along the x axis, with the first 10 in the same
            coordinate system, the last 10 chained with a const offset of 1.
            0 ----------7  8  9 10  11  12  13 -------- 20
            |p0|        |p7|p8|p9|p10|p11|p12|
            let's query for everything
                within [7.5, 12.5] --> expected 8,9,10,11
                intersects ^^ --> expected 7,8,9,10,11,12
        */
        std::set<std::string> expected_within = {{ "p8", "p9", "p10", "p11" }};
        std::set<std::string> expected_intersects = {{ "p7", "p8", "p9", "p10", "p11", "p12" }};

        // within
        auto query = SpatialIndexQuery::withinBox(Eigen::Vector3d{7.5, -1, -1}, Eigen::Vector3d{12.5, 2, 2}, cs);
        core.answerQuery(query);

        //BOOST_CHECK_EQUAL(query->results.size(), expected_within.size());
        for (auto r : query->results)
        {
            BOOST_CHECK( expected_within.find(r->id()) != expected_within.end() );
        }

        // intersects
        query->results.clear();
        query->mode(SpatialIndexQuery::INTERSECTS);
        core.answerQuery(query);

        //BOOST_CHECK_EQUAL(query->results.size(), expected_intersects.size());
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
