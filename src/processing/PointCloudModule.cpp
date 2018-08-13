#include <sempr/processing/PointCloudModule.hpp>

namespace sempr { namespace processing {

PointCloudModule::PointCloudModule()
{
    // nothing to do?
    std::cout << "created PCM" << std::endl;
}

PointCloudModule::~PointCloudModule()
{
    // nothing to do?
}

std::string PointCloudModule::type() const {
    return "PointCloudModule";
}

void PointCloudModule::process(query::PolygonQuery::Ptr query)
{
    query::ObjectQuery<entity::PointCloud>::Ptr clouds(new query::ObjectQuery<entity::PointCloud>() );

    ask(clouds);

    for (auto const& cloud : clouds->results)
    {
        calculatePoints(cloud, query);
    }
}

void PointCloudModule::calculatePoints(const entity::PointCloud::Ptr cloud, query::PolygonQuery::Ptr query)
{
    unsigned int i, j;

    const std::vector <geom::Coordinate>* coords_ptr = query->geometry()->getCoordinates()->toVector();
    const std::vector <geom::Coordinate>& coords = *coords_ptr;

    double minX = coords[0].x;
    double minZ = coords[0].z;

    double maxX = minX;
    double maxZ = minZ;

    for(i = 1; i < coords.size(); i++)
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

    int intersections = 0;

    geom::Coordinate outerPoint(minX - 10, 0, minZ - 10);

    const std::vector <geom::Coordinate>* cloud_coords_ptr = cloud->getGeometry()->getCoordinates()->toVector();
    const std::vector <geom::Coordinate>& cloud_coords = *cloud_coords_ptr;

    for (j = 0; j < cloud_coords.size(); j++)
    {
        if (cloud_coords[j].x < minX || cloud_coords[j].z < minZ || cloud_coords[j].x > maxX || cloud_coords[j].z > maxZ)
        {
            continue;
        }
        else
        {
            for(i = 1; i < coords.size(); i++)
            {
                intersections += checkIntersection(coords[i - 1], coords[i], outerPoint, cloud_coords[j]);
            }
            if((intersections & 1) == 1)
            {
                    query->results.push_back(cloud_coords[j]);
            }
            intersections = 0;
        }
    }
    // this is a really bad running time, isnt it? .. O(NumberOfClouds) * O(NumberOfPointsInEachCloud) * O(NumberOfVerticesInPolygon) * O(calculating) ... :(

}

int PointCloudModule::checkIntersection(geom::Coordinate v11, geom::Coordinate v12, geom::Coordinate v21, geom::Coordinate v22)
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
