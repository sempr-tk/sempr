#include <geos/geom/GeometryFactory.h>
#include "test_utils.hpp"

using namespace testing;

BOOST_AUTO_TEST_SUITE(geometries)
    const std::string db_path = "test_geometry_sqlite.db";  //todo seg. fault after renameing

    BOOST_AUTO_TEST_CASE(geometries_coordinates)
    {
        geom::Coordinate dummyPosition;

        //default constructor dont set NULL!
        dummyPosition.setNull();

        BOOST_CHECK(dummyPosition.isNull());
    }

    BOOST_AUTO_TEST_CASE(geometries_cloneable)
    {
        // test if our CRTP "CloneableGeometry<class T>" works as expected.

        // construct a GEOM Polygon by a Collection:

        std::vector<geom::Geometry*> points;
        float x[] = {0, 0, 1, 1};
        float y[] = {0, 1, 1, 0};
        for (int i = 0; i < 4; i++) {
            geom::Point* p = (geom::Point*) geom::GeometryFactory::getDefaultInstance()->createPoint(geom::Coordinate(x[i], y[i]));
            points.push_back(p);
        }

        auto collectionGeom = geom::GeometryFactory::getDefaultInstance()->createGeometryCollection(&points);    //this will overwrite the old (empty) default collection geometry. Memory Leak! TODO

        auto poly = dynamic_cast<geom::Polygon*>(collectionGeom->convexHull());

        //geom::GeometryFactory::getDefaultInstance()->destroyGeometry(collectionGeom); //will also destroy the polygon points. - crashed!

        // use it in a polygon:
        Polygon::Ptr polygon(new Polygon());
        polygon->setGeometry(poly);

        // get the expected wkt output for comparisons.
        std::string expectedWkt = toString(polygon->getGeometry());
        //print(polygon->getGeometry());

        // now, create different pointer to the same thing.
        Geometry::Ptr geometry = polygon;

        // clone them!
        auto pclone = polygon->clone();   // Polygon::Ptr
        auto gclone = geometry->clone();  // Geometry::Ptr

        // change the original (this will also destroy the source polygon)
        polygon->setGeometry(geom::GeometryFactory::getDefaultInstance()->createPolygon());
        
        // print the clones
        // print(pclone->getGeometry());
        // print(cpclone->getGeometry());
        // print(gclone->getGeometry());
        BOOST_CHECK_EQUAL(expectedWkt, toString(pclone->getGeometry()));
        BOOST_CHECK_EQUAL(expectedWkt, toString(gclone->getGeometry()));
    }


    BOOST_AUTO_TEST_CASE(geometries_well_known_polygon)
    {
        // set up ring
        //
        //           B
        //         /   \ .
        //  ls ---/- m  \ .
        //      A ------ C
        LinearRing::Ptr linearRing(new LinearRing());
        std::vector<geom::Coordinate> ring;
        ring.push_back(geom::Coordinate(-1, -1));
        ring.push_back(geom::Coordinate(+0, +1));
        ring.push_back(geom::Coordinate(+1, -1));
        ring.push_back(geom::Coordinate(-1, -1));   //needed to close the ring!
        linearRing->setCoordinates(ring);

        // set up polygon from ring
        Polygon::Ptr polygon(new Polygon());
        polygon->setCoordinates(ring);

        auto bufferWKB = Geometry::exportToWKB(polygon->getGeometry());
        auto importedWKB = Geometry::importFromWKB(bufferWKB);

        BOOST_CHECK(importedWKB->equals(polygon->getGeometry()));


        auto bufferWKT = Geometry::exportToWKT(polygon->getGeometry());
        auto importedWKT = Geometry::importFromWKT(bufferWKT);

        BOOST_CHECK(importedWKT->equals(polygon->getGeometry()));
    }

    BOOST_AUTO_TEST_CASE(geometries_well_known_multipoint)
    {
        
         // create a bunch of geometries
        MultiPoint::Ptr multiPoint(new MultiPoint());

        std::vector<geom::Coordinate> coords;
        for(int i = 0; i < 10000; i++)
        {
            // Random numbers: Min i ... Max 10 + 1
            double x = rand() % (10 + i) + i;
            double y = rand() % (10 + i) + i;
            double z = rand() % (10 + i) + i;
            coords.push_back(geom::Coordinate(x, y, z));
        }

        multiPoint->setCoordinates(coords);

        auto bufferWKB = Geometry::exportToWKB(multiPoint->getGeometry());
        auto importedWKB = Geometry::importFromWKB(bufferWKB);

        BOOST_CHECK(importedWKB->equals(multiPoint->getGeometry()));


        auto bufferWKT = Geometry::exportToWKT(multiPoint->getGeometry());
        auto importedWKT = Geometry::importFromWKT(bufferWKT);

        BOOST_CHECK(importedWKT->equals(multiPoint->getGeometry()));
    }


    BOOST_AUTO_TEST_CASE(geometries_insertion)
    {
        Core core;

        ActiveObjectStore::Ptr active(new ActiveObjectStore());
        core.addModule(active);

        ODBStorage::Ptr storage = setUpStorage(db_path, true);
        DBUpdateModule::Ptr updater(new DBUpdateModule(storage));
        core.addModule(updater);

        // create a bunch of geometries
        
        // set up the point
        Point::Ptr point(new Point());
        point->setCoordinate(geom::Coordinate(0, 0));


        // set up ring
        //
        //           B
        //         /   \ .
        //  ls ---/- m  \ .
        //      A ------ C
        LinearRing::Ptr linearRing(new LinearRing());
        std::vector<geom::Coordinate> ring;
        ring.push_back(geom::Coordinate(-1, -1));
        ring.push_back(geom::Coordinate(+0, +1));
        ring.push_back(geom::Coordinate(+1, -1));
        ring.push_back(geom::Coordinate(-1, -1));   //needed to close the ring!
        linearRing->setCoordinates(ring);

        // set up polygon from ring
        Polygon::Ptr polygon(new Polygon());
        polygon->setCoordinates(ring);

        /* Alternative way to set the geometry manually with a given linear ring:
        std::vector<geom::Geometry*> holes;
        auto polygonGeometry = geom::GeometryFactory::getDefaultInstance()->createPolygon(*linearRing->getGeometry(), holes);  //linear Ring as DeepCopy
        polygon->setGeometry(polygonGeometry);
        */


        // set up the linestring
        LineString::Ptr linestring(new LineString());
        std::vector<geom::Coordinate> string;
        string.push_back(geom::Coordinate(-5, 0));
        string.push_back(geom::Coordinate(+0, 0));
        linestring->setCoordinates(string);

        // set up a multi point
        MultiPoint::Ptr multiPoint(new MultiPoint());
        multiPoint->setCoordinates(ring);

        // build a geometry collection with the point, linestring and polygon
        std::vector<geom::Geometry*> geoms;
        geoms.push_back(point->getGeometry()->clone());
        geoms.push_back(linestring->getGeometry()->clone());
        geoms.push_back(linearRing->getGeometry()->clone());
        geoms.push_back(polygon->getGeometry()->clone());

        GeometryCollection::Ptr collection(new GeometryCollection());
        collection->setCollection(geoms);

        // insert 
        core.addEntity(point);
        core.addEntity(linestring);
        core.addEntity(linearRing);
        core.addEntity(polygon);
        core.addEntity(collection);
        core.addEntity(multiPoint);
    }

    BOOST_AUTO_TEST_CASE(geometries_operations)
    {
        ODBStorage::Ptr storage = loadStorage(db_path);

        std::vector<Point::Ptr> points;
        std::vector<LineString::Ptr> lstrings;
        std::vector<LinearRing::Ptr> lrings;
        std::vector<Polygon::Ptr> polygons;
        std::vector<GeometryCollection::Ptr> collections;
        std::vector<MultiPoint::Ptr> multiPoints;

        storage->loadAll(points);
        storage->loadAll(lstrings);
        storage->loadAll(lrings);
        storage->loadAll(polygons);
        storage->loadAll(collections);
        storage->loadAll(multiPoints);

        BOOST_CHECK_EQUAL(points.size(), 1);
        BOOST_CHECK_EQUAL(lstrings.size(), 1);
        BOOST_CHECK_EQUAL(lrings.size(), 1);
        BOOST_CHECK_EQUAL(polygons.size(), 1);
        BOOST_CHECK_EQUAL(collections.size(), 1);
        BOOST_CHECK_EQUAL(multiPoints.size(), 1);

        BOOST_CHECK(points[0]->getGeometry()->within(polygons[0]->getGeometry()));
        BOOST_CHECK(lstrings[0]->getGeometry()->intersects(polygons[0]->getGeometry()));
        
    }

    BOOST_AUTO_TEST_CASE(geometries_transformation)
    {
        LocalCS::Ptr identity(new LocalCS());

        Point::Ptr point2D(new Point());
        point2D->setCoordinate(geom::Coordinate(1, 2));
        point2D->setCS(identity);

        Point::Ptr point3D(new Point());
        point3D->setCoordinate(geom::Coordinate(1, 2, 3));
        point3D->setCS(identity);

        LocalCS::Ptr transformation(new LocalCS());
        transformation->setRotation(0, 0, 1, M_PI/2);
        transformation->setTranslation(2, 2, 0);
        transformation->setParent(identity);

        point2D->transformToCS(transformation);
        point3D->transformToCS(transformation);

        Point::Ptr pointExpected(new Point());
        pointExpected->setCoordinate(geom::Coordinate(0, 1));

        BOOST_CHECK(pointExpected->getGeometry()->equalsExact(point2D->getGeometry(), 0.0001));
        BOOST_CHECK(pointExpected->getGeometry()->equalsExact(point3D->getGeometry(), 0.0001));   //Note this will only compare 2 dimension!
    }


    BOOST_AUTO_TEST_CASE(geometries_cleanup)
    {
        removeStorage(db_path);
    }

    BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(reference_systems)
    BOOST_AUTO_TEST_CASE(local_rotate_and_translate)
    {
        /*
            A --- rot 90deg, trans (1 1) --> B

            AY                BX
            ^                 ^
            |                 |
            |                 |
            |                 |
            |    BY <---------+ (AX=1, AY=1)
            |
            |
            |
            |
            +-----------------------------------> AX

            UnitXA = (1 0)_{A}  ==  (-1 0)_{B}
            UnitXB = (1 0)_{B}  ==  (1 2)_{A}

        */
        LocalCS::Ptr a(new LocalCS());
        LocalCS::Ptr b(new LocalCS());

        b->setParent(a);
        b->setRotation(0, 0, 1, M_PI/2.);
        b->setTranslation(1, 1, 0);

        Eigen::Vector3d unitXA(1, 0, 0);
        Eigen::Vector3d unitXB(1, 0, 0);
        Eigen::Vector3d expectedAinB(-1, 0, 0);
        Eigen::Vector3d expectedBinA(1, 2, 0);

        auto AtoB = b->transformationFromRoot();
        auto BtoA = b->transformationToRoot();

        auto AinB = AtoB*unitXA;
        auto BinA = BtoA*unitXB;

        BOOST_CHECK(AinB.isApprox(expectedAinB));
        BOOST_CHECK(BinA.isApprox(expectedBinA));
        BOOST_CHECK(b->isChildOf(a));
        BOOST_CHECK(!a->isChildOf(b));
    }

    BOOST_AUTO_TEST_CASE(local_chain_translate)
    {
        /*
         1          ,---- c
         0  a <--- b
        -1          `---- d --- e
            0      1      2     3
        */
        LocalCS::Ptr a(new LocalCS());
        LocalCS::Ptr b(new LocalCS());
        LocalCS::Ptr c(new LocalCS());
        LocalCS::Ptr d(new LocalCS());
        LocalCS::Ptr e(new LocalCS());

        b->setParent(a);
            c->setParent(b);
            d->setParent(b);
                e->setParent(d);

        BOOST_CHECK(b->isChildOf(a));
        BOOST_CHECK(e->isChildOf(a));
        BOOST_CHECK(c->isChildOf(a));
        BOOST_CHECK(!c->isChildOf(d));

        b->setTranslation(1, 0, 0);
        c->setTranslation(1, 1, 0);
        d->setTranslation(1, -1, 0);
        e->setTranslation(1, 0, 0);

        auto fromC = c->transformationToRoot();
        auto toE = e->transformationFromRoot();

        /*
            origin of c is at (2 1)
            origin of e is at (3 -1)
            so, (0 0) in c is (-1 2) in e
        */
        Eigen::Vector3d inC(0, 0, 0);
        auto inE = toE * fromC * inC;
        std::cout << inC.transpose() << " in C is " << inE.transpose() << " in E" << '\n';

        Eigen::Vector3d expected(-1, 2, 0);
        BOOST_CHECK(inE.isApprox(expected));
    }

    BOOST_AUTO_TEST_CASE(local_chain_transrot)
    {
        /*
             ,--- translate (1 0 0) -- B
            A
             `--- rotate 90 deg (around Z axis) -- C -- translate(1 0 0) -- D
              `----- rotate 90 deg & translate(1 0 0) -- E
               `----- translate(1 0 0) --- F --- rotate 90 deg -- G

            (0 0) in B
            is (1 0) in A
            is (0 -1) in C
            is (-1 -1) in D
            is (0 0) in E       (!)
            is (0 0) in F
            is (0 0) in G

            (!) Note:
                A -- rotate --> FOO --> translate --> B
                is _not_ the same as
                A -- rotate & translate --> B
                though, when looking at a transformation matrix
                ( R | T )
                (---+---)
                ( 0 | 1 )
                the rotation is applied first. But: In the chain (-- rotate --> -- translate -->)
                the translation is expressed in the rotated coordinate system, whereas in
                (-- rotate & translate -->), both rotation and translation refer to the same base!
                (Maybe this is trivial to you, but I stumbled over it...)

            (1 0) in G
            is (0 1) in F
            is (1 1) in A
        */
        LocalCS::Ptr a(new LocalCS());
        LocalCS::Ptr b(new LocalCS());
        LocalCS::Ptr c(new LocalCS());
        LocalCS::Ptr d(new LocalCS());
        LocalCS::Ptr e(new LocalCS());
        LocalCS::Ptr ecpy(new LocalCS());
        LocalCS::Ptr f(new LocalCS());
        LocalCS::Ptr g(new LocalCS());

        b->setParent(a); b->setTranslation(1, 0, 0);
        c->setParent(a); c->setRotation(0, 0, 1, M_PI/2.);
        d->setParent(c); d->setTranslation(1, 0, 0);

        e->setParent(a); e->setRotation(0, 0, 1, M_PI/2.); e->setTranslation(1, 0, 0);
        ecpy->setParent(a); ecpy->setTranslation(1, 0, 0); ecpy->setRotation(0, 0, 1, M_PI/2.);
        // order of rot/trans in one frame should be irrelevant
        BOOST_CHECK(e->transformationToRoot().isApprox(ecpy->transformationToRoot()));

        f->setParent(a); f->setTranslation(1, 0, 0);
        g->setParent(f); g->setRotation(0, 0, 1, M_PI/2.);


        /** (0 0) in B to C, D, E */
        Eigen::Vector3d inB(0, 0, 0); // is (1 0 0) in A, should be (0 -1 0) in C
        auto fromB = b->transformationToRoot();
        auto toC = c->transformationFromRoot();
        auto toD = d->transformationFromRoot();
        auto toE = e->transformationFromRoot();

        auto inC = toC * fromB * inB;
        auto inD = toD * fromB * inB;
        auto inE = toE * fromB * inB;
        // std::cout << inB.transpose() << " in B is " << inC.transpose() << " in C" << '\n';

        Eigen::Vector3d expectedC(0, -1, 0);
        Eigen::Vector3d expectedD(-1, -1, 0);
        Eigen::Vector3d expectedE(0, 0, 0);

        BOOST_CHECK(inC.isApprox(expectedC));
        BOOST_CHECK(inD.isApprox(expectedD));
        BOOST_CHECK(inE.isApprox(expectedE));

        /** (1 0) in G to F, A */
        Eigen::Vector3d inG(1, 0, 0);
        auto fromG = g->transformationToRoot();
        auto toF = f->transformationFromRoot();
        auto toA = a->transformationFromRoot();

        auto inF = toF * fromG * inG;
        auto inA = toA * fromG * inG;

        Eigen::Vector3d expectedF(0, 1, 0);
        Eigen::Vector3d expectedA(1, 1, 0);

        BOOST_CHECK(inF.isApprox(expectedF));
        BOOST_CHECK(inA.isApprox(expectedA));
    }


    BOOST_AUTO_TEST_CASE(global_references)
    {
        // ProjectionCS::Ptr projCC = ProjectionCS::CreateEquirect(0, 0);
        GeodeticCS::Ptr wgs84(new GeodeticCS());
        ProjectionCS::Ptr utm(new UniversalTransverseMercatorCS(32));

        auto wgs2utm = wgs84->to(utm);

        BOOST_CHECK(!wgs2utm.empty());

        // mannheim paradeplatz (wiki example)
        // wgs85: 49.487111 (lat), 8.466278 (lon)
        // mgrs: zone 32U, E: 61344 N: 81745
        // TODO: GDAL only knows UTM zones __N and __S (north/south), but not the local definitions made by UTMREF / MGRS (e.g. no zone 32U, only 32)

        Point::Ptr p(new Point());
        geos::geom::Point* wktPoint = dynamic_cast<geos::geom::Point*>(Geometry::importFromWKT("POINT (49.487111 8.466278)"));
        p->setGeometry(wktPoint);
        p->setCS(wgs84);

        // transform
        p->transformToCS(utm);

        //print(p->getGeometry());

        BOOST_CHECK_CLOSE(p->getGeometry()->getX(),  461344., 0.0001);
        BOOST_CHECK_CLOSE(p->getGeometry()->getY(), 5481745., 0.0001);
    }

    BOOST_AUTO_TEST_CASE(global_mgrs_check)
    {
        std::string mgrsQ = MilitaryGridReferenceSystem::buildMGRS("Z");    // somewhere north of the russian arctic

        std::string gzd;
        double x, y;
        MilitaryGridReferenceSystem::splitMGRS(mgrsQ, gzd, x, y);    // this will throw an exception if something is wrong. Unhandeld to test!

        MilitaryGridReferenceSystem::splitMGRS("32U", gzd, x, y); 
        BOOST_CHECK(gzd == "32U");
        BOOST_CHECK(x == 0);
        BOOST_CHECK(y == 0);

        // check 1 digit zone and precision correction
        MilitaryGridReferenceSystem::splitMGRS("4QFJ 12345 67890", gzd, x, y); 
        std::string mgrs4Q = MilitaryGridReferenceSystem::buildMGRS(gzd, x, y);
        BOOST_CHECK(mgrs4Q == "4QFJ 12345000 67890000");

        // example mgrs usage:
        geos::geom::Coordinate coord;
        std::string zoneGZD;
        MilitaryGridReferenceSystem::splitMGRS("32UMC3135061510", zoneGZD, coord.x, coord.y); 

        ProjectionCS::Ptr mgrsCS(new MilitaryGridReferenceSystem(zoneGZD));

        Point::Ptr p(new Point());
        p->setCoordinate(coord);
        p->setCS(mgrsCS);

        GeodeticCS::Ptr wgs84(new GeodeticCS());
        p->transformToCS(wgs84);

        BOOST_CHECK_CLOSE(p->getGeometry()->getX(), 52, 0.0001);
        BOOST_CHECK_CLOSE(p->getGeometry()->getY(), 8, 0.0001);
    }

    BOOST_AUTO_TEST_CASE(geometry_transformation_local)
    {
        LocalCS::Ptr root(new LocalCS());
        LocalCS::Ptr frame(new LocalCS());
        frame->setParent(root);
        frame->setRotation(0, 0, 1, M_PI/2.); // 90 deg round Z-axis
        frame->setTranslation(1, 0, 0); // shift 1 along X

        Point::Ptr p(new Point());
        p->setCoordinate(geom::Coordinate(1, 0));

        // apply rotation
        p->setCS(frame);
        p->transformToCS(root);

        // expect (1 1)
        BOOST_CHECK_CLOSE(p->getGeometry()->getX(), 1, 0.000001);
        BOOST_CHECK_CLOSE(p->getGeometry()->getY(), 1, 0.000001);
    }

    BOOST_AUTO_TEST_CASE(geometry_transformation_global_local_combined)
    {
        /*
            Create two root-coordinate systems:
            1. wgs84
            2. equirectangular projection at 52N 8E

            Then, apply a local chain to (2):

            (2) --rotate left--> (3) --translate (100, 200)--> (4) --rotate right--> (5)

            lat: 52, lon: 8 in (1)
            is (0 0) in (2)
            is (0 0) in (3)
            is (-100, -200) in (4)
            is (200, -100) in (5)
        */
        GeodeticCS::Ptr wgs84(new GeodeticCS());
        GeocentricCS::Ptr centric(new LocalTangentPlaneCS(52, 8));

        LocalCS::Ptr localRot(new LocalCS());
        localRot->setParent(centric);
        localRot->setRotation(0, 0, 1, M_PI/2.);

        LocalCS::Ptr localTrans(new LocalCS());
        localTrans->setParent(localRot);
        localTrans->setTranslation(100, 200, 0);
        localTrans->setRotation(0, 0, 1, -M_PI/2.);


        /*
            Create a point in WGS84 at the center of the equirectangular projection.
            --> (0,0) in equi --> (0, 0) in rot --> (200, -100) in transrot
        */
        Point::Ptr p(new Point());
        std::string latlon = "POINT (52 8)"; // lat, lon
        geos::geom::Point* wktPoint = dynamic_cast<geos::geom::Point*>(Geometry::importFromWKT(latlon));

        p->setGeometry(wktPoint);

        p->setCS(wgs84);

        BOOST_CHECK_CLOSE(p->getGeometry()->getX(), 52, 0.0000001);
        BOOST_CHECK_CLOSE(p->getGeometry()->getY(),  8, 0.0000001);

        p->transformToCS(localTrans);

        BOOST_CHECK_CLOSE(p->getGeometry()->getX(),  200, 0.0000001);
        BOOST_CHECK_CLOSE(p->getGeometry()->getY(), -100, 0.0000001);
        
    }

BOOST_AUTO_TEST_SUITE_END()
