#include <sempr/query/PolygonQuery.hpp>
#include <sempr/entity/spatial/GeometryCollection.hpp>

namespace sempr { namespace query {

PolygonQuery::PolygonQuery()
{
}

PolygonQuery::PolygonQuery(const geom::Polygon *poly) : m_poly(poly)
{
    results.reserve(1); //what is a useful size here?!
}

PolygonQuery::PolygonQuery(entity::agro::AgroEntity::Ptr &agro) : PolygonQuery(agro->geometry())
{
}

PolygonQuery::PolygonQuery(entity::Polygon::Ptr &polygon) : PolygonQuery(polygon->geometry())
{
}

PolygonQuery::~PolygonQuery()
{
}
/*
PolygonQuery::Ptr PolygonQuery::createQuery(entity::agro::AgroEntity::Ptr agro)
{
    return createQuery(agro->geometry());
}

PolygonQuery::Ptr PolygonQuery::createQuery(entity::Polygon::Ptr polygon)
{
    return createQuery(polygon->geometry());
}

PolygonQuery::Ptr PolygonQuery::createQuery(const geom::Polygon* polygon)
{
    PolygonQuery::Ptr query(new PolygonQuery(polygon));
        
    return query;
}
*/


void PolygonQuery::calculatePoints(const entity::PointCloud2::Ptr cloud)
{
     unsigned int i, j;
    // get the Boundingbox
    //geom::Envelope bb = polygon->getEnvelope();          //this is fine.
    //polygon->envelope->;
    //geom::Coordinate outerPoint;

    //outerPoint.x = bb.MinX - 10;
    //outerPoint.y = 0;
    //outerPoint.z = bb.MinZ - 10;

    //TODOOOOO
    //OGRPointIterator *it = polygon->getExteriorRing()->getPointIterator();
    //geom::Coordinate *p = new Coordinate();
    //geom::CoordinateSequence* coords = polygon->getCoordinates();
    const std::vector <geom::Coordinate>* coords_ptr = m_poly->getCoordinates()->toVector();
    const std::vector <geom::Coordinate>& coords = *coords_ptr;

    double minX = coords[0].x;
    double minZ = coords[0].z;

    double maxX = minX;
    double maxZ = minZ;

    //std::vector<OGRPoint *> polygon_verts(agro->geometry()->getExteriorRing()->getNumPoints());
    //std::vector<entity::PointCloud::Point> p_verts(polygon->getExteriorRing()->getNumPoints());
    //entity::PointCloud::Point tmp;
    for(i = 1; i < coords.size() - 1; i++)
    {
        if(coords[i].x < minX)
            minX = coords[i].x;
        if(coords[i].z < minZ)
            minZ = coords[i].z;
        if(coords[i].x > maxX)
            maxX = coords[i].x;
        if(coords[i].z > maxZ)
            maxZ = coords[i].z;
    }

    //outerPoint.x = bb.MinX - 10;
    //outerPoint.y = 0;
    //outerPoint.z = bb.MinZ - 10;
    // do this just one time.
    int intersections = 0;

    //for (auto const& pc : m_clouds);
    // get active object store
    //processing::ActiveObjectStore::Ptr aos = m_core.getModule<processing::ActiveObjectStore::Ptr>();
    //aos->process();
    //ObjectQuery<entity::PointCloud>::Ptr clouds(new ObjectQuery<entity::PointCloud>() );

    geom::Coordinate outerPoint(minX - 10, 0, minZ - 10);

    const std::vector <geom::Coordinate>* cloud_coords_ptr = cloud->geometry()->getCoordinates()->toVector();
    const std::vector <geom::Coordinate>& cloud_coords = *cloud_coords_ptr;
    //m_core.answerQuery(clouds);     // TODO Access the core ... hmm

    for (j = 0; j < cloud_coords.size(); j++)
    {
        if (cloud_coords[j].x < minX || cloud_coords[j].z < minZ || cloud_coords[j].x > maxX || cloud_coords[j].z > maxZ)
        {
            //std::cout << "Point: (" << point.x << "," << point.z << ") is not in ([" << bb.minX << "," << bb.maxX << "],[" << bb.minZ << "," << bb.maxZ << "]" << std::endl;
            continue;
        }
        else
        {
            //std::cout << "Point: (" << point.x << "," << point.z << ") is in BB ([" << bb.minX << "," << bb.maxX << "],[" << bb.minZ << "," << bb.maxZ << "]" << std::endl;
            for(i = 1; i < coords.size(); i++)
            {
                intersections += checkIntersection(coords[i - 1], coords[i], outerPoint, cloud_coords[j]);
            }
            qDebug() << intersections;
            if((intersections & 1) == 1)
            {
                    //std::cout << "Match" << std::endl;
                    results.push_back(cloud_coords[j]);
            }
                //Ich bin dumm. Warum vergesse ich sowas nur. :D
            intersections = 0;

        }
        std::cout << results.size() << std::endl;

    }
    // this is a really bad running time .. O(NumberOfClouds) * O(NumberOfPointsInEachCloud) * O(NumberOfVerticesInPolygon) * O(calculating) ... :(

}


//int PolygonQuery::checkIntersection(entity::PointCloud::Point v11, entity::PointCloud::Point v12, entity::PointCloud::Point v21, entity::PointCloud::Point v22)
int PolygonQuery::checkIntersection(geom::Coordinate v11, geom::Coordinate v12, geom::Coordinate v21, geom::Coordinate v22)
{
    double a1, a2;
    double b1, b2;
    double c1, c2;
    double d1, d2;

    a1 = v12.z - v11.z;
    b1 = v11.x - v12.x;
    c1 = (v12.x * v11.z) - (v11.x * v12.z); //cross-product?

    d1 = (a1 * v21.x) + (b1 * v21.z) + c1;
    d2 = (a1 * v22.x) + (b1 * v22.z) + c1;

    if ((d1 > 0 && d2 > 0) || (d1 < 0 && d2 < 0))
    {
        return 0;
    }

    a2 = v22.z - v21.z;
    b2 = v21.x - v22.x;
    c2 = (v22.x * v21.z) - (v21.x * v22.z); //cross-product?

    d1 = (a2 * v11.x) + (b2 * v11.z) + c2;
    d2 = (a2 * v12.x) + (b2 * v12.z) + c2;

    if ((d1 > 0 && d2 > 0) || (d1 < 0 && d2 < 0))
    {
        return 0;
    }

    if ((a1 * b2) - (a2 * b1) == 0.0)
    {
        return 0;
    }

    return 1;
}


}}
