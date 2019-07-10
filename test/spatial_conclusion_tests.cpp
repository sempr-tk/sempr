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

std::vector<geos::geom::Coordinate> getVechtaCoords()
{
    std::vector<geom::Coordinate> vechtaCorner;
    
    vechtaCorner.emplace_back(52.755742, 8.274784);
    vechtaCorner.emplace_back(52.758035, 8.307160);
    vechtaCorner.emplace_back(52.745131, 8.328818);
    vechtaCorner.emplace_back(52.701042, 8.299618);
    vechtaCorner.emplace_back(52.719873, 8.257034);
    vechtaCorner.emplace_back(52.755742, 8.274784); //close the ring

    return vechtaCorner;
}

std::vector<geos::geom::Coordinate> getOsnaCoords()
{
    std::vector<geom::Coordinate> osnaCorner;
    
    osnaCorner.emplace_back(52.245902, 8.087810);
    osnaCorner.emplace_back(52.302939, 8.034527);
    osnaCorner.emplace_back(52.297650, 8.107368);
    osnaCorner.emplace_back(52.245900, 8.087793);
    osnaCorner.emplace_back(52.245902, 8.087810); //close the ring

    return osnaCorner;
}

std::vector<geos::geom::Coordinate> getBremenCoords()
{
    std::vector<geom::Coordinate> bremenCorner;
    bremenCorner.emplace_back(53.096611, 8.688279);
    bremenCorner.emplace_back(53.146918, 8.733290);
    bremenCorner.emplace_back(53.110185, 8.867102);
    bremenCorner.emplace_back(53.041551, 8.983798);
    bremenCorner.emplace_back(53.029056, 8.858612);
    bremenCorner.emplace_back(53.096611, 8.688279); //close the ring

    return bremenCorner;
}

std::vector<geos::geom::Coordinate> getNDSCoords()
{
    // counter clock wise
    std::vector<geom::Coordinate> ndsCorner;
    ndsCorner.emplace_back(52.241226, 7.0658);
    ndsCorner.emplace_back(52.379696, 7.572223);
    ndsCorner.emplace_back(52.474945, 7.603808);
    ndsCorner.emplace_back(52.372092, 7.804957);
    ndsCorner.emplace_back(52.365060, 7.9367);
    ndsCorner.emplace_back(52.268337, 7.926824);
    ndsCorner.emplace_back(52.365060, 7.93667);
    ndsCorner.emplace_back(52.083415, 7.887888);
    ndsCorner.emplace_back(52.075998, 8.196700);
    ndsCorner.emplace_back(52.198737, 8.460202);
    ndsCorner.emplace_back(52.454954, 8.332350);
    ndsCorner.emplace_back(52.500513, 8.558474);
    ndsCorner.emplace_back(52.531468, 8.652268); //NRW Northpoint
    ndsCorner.emplace_back(52.400771, 8.724932);
    ndsCorner.emplace_back(52.401876, 8.93696);
    ndsCorner.emplace_back(52.497824, 9.095536);
    ndsCorner.emplace_back(52.265462, 8.971608);
    ndsCorner.emplace_back(51.854584, 9.328392);
    ndsCorner.emplace_back(51.862115, 9.459723);
    ndsCorner.emplace_back(51.650059, 9.374471);
    ndsCorner.emplace_back(51.612036, 9.540668);
    ndsCorner.emplace_back(51.340252, 9.568025);
    ndsCorner.emplace_back(51.587427, 10.373788);
    ndsCorner.emplace_back(51.566889, 10.658262);
    ndsCorner.emplace_back(52.009182, 10.564431);
    ndsCorner.emplace_back(52.056388, 10.964671);
    ndsCorner.emplace_back(52.218803, 11.059229); //A2 Border
    ndsCorner.emplace_back(52.842004, 10.764717);
    ndsCorner.emplace_back(53.036002, 11.597555); //Elbe
    ndsCorner.emplace_back(53.453140, 10.078754);
    ndsCorner.emplace_back(53.414084, 9.978458);
    ndsCorner.emplace_back(53.466578, 9.806722);
    ndsCorner.emplace_back(53.554267, 9.774212);
    ndsCorner.emplace_back(53.610751, 9.562691);
    ndsCorner.emplace_back(53.877164, 9.230297);
    ndsCorner.emplace_back(53.905287, 8.647583);
    ndsCorner.emplace_back(53.662161, 8.455094);
    ndsCorner.emplace_back(53.607053, 8.514606);
    ndsCorner.emplace_back(53.603353, 8.65325);
    ndsCorner.emplace_back(53.441515, 8.582983);
    ndsCorner.emplace_back(53.215264, 8.656875);
    ndsCorner.emplace_back(53.052565, 9.030710);
    ndsCorner.emplace_back(53.007938, 8.939740);
    ndsCorner.emplace_back(53.030989, 8.735467);
    ndsCorner.emplace_back(53.170472, 8.611213);
    ndsCorner.emplace_back(53.174157, 8.495368);
    ndsCorner.emplace_back(53.549985, 8.511206);
    ndsCorner.emplace_back(53.549985, 8.511206);
    ndsCorner.emplace_back(53.521320, 8.232034);
    ndsCorner.emplace_back(53.522269, 8.311878);
    ndsCorner.emplace_back(53.398310, 8.250894);
    ndsCorner.emplace_back(53.452971, 8.136036);
    ndsCorner.emplace_back(53.448714, 8.094245);
    ndsCorner.emplace_back(53.501105, 8.060751);
    ndsCorner.emplace_back(53.542582, 8.1698);
    ndsCorner.emplace_back(53.707251, 8.03243);
    ndsCorner.emplace_back(53.663311, 7.231965);
    ndsCorner.emplace_back(53.349005, 6.991342);
    ndsCorner.emplace_back(53.328303, 7.244930);
    ndsCorner.emplace_back(53.237153, 7.205646);
    ndsCorner.emplace_back(52.635858, 7.045900);
    ndsCorner.emplace_back(52.647768, 6.755056);
    ndsCorner.emplace_back(52.486377, 6.697683);
    ndsCorner.emplace_back(52.429083, 7.010758);
    ndsCorner.emplace_back(52.241226, 7.0658);   //close

    return ndsCorner;
}


BOOST_AUTO_TEST_SUITE(spatial_conclusion)
    std::string dbfile = "test_spatial_conclusion_sqlite.db";

    BOOST_AUTO_TEST_CASE(spatial_conclusion_id_uri)
    {
        GeometricObject::Ptr farfaraway( new GeometricObject() );

        auto uri = sempr::buildURI(farfaraway->id(), sempr::baseURI());
        auto id = sempr::extractID(uri, sempr::baseURI());

        BOOST_CHECK_EQUAL(id, farfaraway->id());
    }

    BOOST_AUTO_TEST_CASE(spatial_conclusion_cities)
    {
        Core core;
        
        GeodeticCS::Ptr globalCS(new GeodeticCS());

        SpatialIndex2D::Ptr index(new SpatialIndex2D(globalCS));
        core.addModule(index);

        SpatialConclusion2D<GeometricObject>::Ptr conclusion(new SpatialConclusion2D<GeometricObject>(index));
        core.addModule(conclusion);

        SopranoModule::Ptr soprano(new SopranoModule());
        core.addModule(soprano);

        GeometricObject::Ptr osna( new GeometricObject() );
        {
            Polygon::Ptr osnaPolygon( new Polygon() );
            osnaPolygon->setCoordinates(getOsnaCoords());
            osnaPolygon->setCS(globalCS);
            core.addEntity(osnaPolygon);    //will not be added by the super object!

            osna->geometry(osnaPolygon);
        }
        core.addEntity(osna);

        GeometricObject::Ptr vechta( new GeometricObject() );
        {
            Polygon::Ptr vechtaPolygon( new Polygon() );
            vechtaPolygon->setCoordinates(getVechtaCoords());
            vechtaPolygon->setCS(globalCS);
            core.addEntity(vechtaPolygon);    //will not be added by the super object!

            vechta->geometry(vechtaPolygon);
        }
        core.addEntity(vechta);

        GeometricObject::Ptr bremen( new GeometricObject() );
        {
            Polygon::Ptr bremenPolygon( new Polygon() );
            bremenPolygon->setCoordinates(getBremenCoords());
            bremenPolygon->setCS(globalCS);
            core.addEntity(bremenPolygon);  //will not be added by the super object!

            bremen->geometry(bremenPolygon);
        }
        core.addEntity(bremen);

        Polygon::Ptr ndsPolygon( new Polygon() );
        ndsPolygon->setCoordinates(getNDSCoords());
        ndsPolygon->setCS(globalCS);
        auto queryNDS = SpatialIndexQuery2D::intersects(ndsPolygon);

        core.answerQuery(queryNDS);
        BOOST_CHECK_EQUAL(queryNDS->results.size(), 2); // Osna, Vechta and Bremen are in NDS if the query use a box. But in real Bremen is no a part of NDS.

        auto vechtaContext = conclusion->getConclusion(vechta);

        BOOST_CHECK_EQUAL(vechtaContext->size(), 4);  // In the view of Vechta, Bremen is in the north east and Osna in the south west.

        SPARQLQuery::Ptr query(new SPARQLQuery());
        query->prefixes["spatial"] = "http://jena.apache.org/spatial#";
        query->query = "SELECT ?o WHERE { ?o spatial:south " + sempr::buildURI(bremen->id(), sempr::baseURI()) + " . }";
        core.answerQuery(query);

        BOOST_CHECK_EQUAL(query->results.size(), 2);    // Vechta and Osnabrück are in the south of Bremen.

        // Add NDS as GeometricObject
        GeometricObject::Ptr nds( new GeometricObject() );
        nds->geometry(ndsPolygon);
        core.addEntity(ndsPolygon);
        core.addEntity(nds);

        // Get current context of Vechta
        vechtaContext = conclusion->getConclusion(vechta);
        BOOST_CHECK_EQUAL(vechtaContext->size(), 4+2);  // In is within and intersects nds

        // Remove Bremen
        bremen->geometry()->removed();
        bremen->removed();
        //delete bremen.get();

        vechtaContext = conclusion->getConclusion(vechta);
        BOOST_CHECK_EQUAL(vechtaContext->size(), 4);  // In is within and intersects nds

    }

    BOOST_AUTO_TEST_CASE(spatial_conclusion_cleanup)
    {
        removeStorage(dbfile);
    }

BOOST_AUTO_TEST_SUITE_END()
