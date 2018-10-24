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
        cloud->loaded();
        
        const std::vector <geom::Coordinate>* coords_ptr = query->geometry()->getCoordinates()->toVector();
        const std::vector <geom::Coordinate>& coords = *coords_ptr;


        geom::Geometry* geom = cloud->getGeometry()->convexHull();
        std::cout << "Boundary NP " << geom->getNumPoints() << std::endl;
    
        const std::vector<geos::geom::Coordinate>* bounds_ptr = geom->getCoordinates()->toVector();
        const std::vector<geos::geom::Coordinate>& bounds = *bounds_ptr;
        
        if(intersect(bounds, coords) == true)
        {
            std::cout << "true" << std::endl;
            calculatePoints(cloud, query);
        }
    }
}

void PointCloudModule::calculatePoints(const entity::PointCloud::Ptr cloud, query::PolygonQuery::Ptr query)
{
    unsigned int i, j;
    bool colors = false;

    double low = query->low();
    double high = query->high();

    const std::vector <geom::Coordinate>* coords_ptr = query->geometry()->getCoordinates()->toVector();
    const std::vector <geom::Coordinate>& coords = *coords_ptr;
    
    if(cloud->hasChannel(10) && cloud->hasChannel(11) && cloud->hasChannel(12))
    {
        std::cout << "we have color" << std::endl;

        colors = true;
    }

    std::vector<geom::Coordinate> c;
    std::vector<double> r;
    std::vector<double> g;
    std::vector<double> b;

    double minX = coords[0].x;
    double minZ = coords[0].z;

    double maxX = minX;
    double maxZ = minZ;

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
                if(cloud_coords[j].y >= low && cloud_coords[j].y <= high)
                {
                    c.push_back(cloud_coords[j]);
                    if(colors == true)
                    {
                        r.emplace_back(cloud->getChannel(10)[j]);
                        g.emplace_back(cloud->getChannel(11)[j]);
                        b.emplace_back(cloud->getChannel(12)[j]);
                    }
                }
            }
            //std::cout << intersections << std::endl;
            intersections = 0;
        }

    }

    entity::PointCloud::Ptr entity = entity::PointCloud::Ptr(new entity::PointCloud());

    entity->setCoordinates(c);
    if(colors == true)
    {
        std::cout << "Set Colors" << std::endl;
        entity->setChannel(10, r);
        entity->setChannel(11, g);
        entity->setChannel(12, b);
    }

    query->results = entity;
    // this is a really bad running time, isnt it? .. O(NumberOfClouds) * O(NumberOfPointsInEachCloud) * O(NumberOfVerticesInPolygon) * O(calculating) ... :(

}

/*
void PointCloudModule::calculatePoints(const entity::PointCloud::Ptr cloud, query::PolygonQuery::Ptr query)
{
    unsigned int i, j;
    bool colors = false;

    const std::vector <geom::Coordinate>* coords_ptr = query->geometry()->getCoordinates()->toVector();
    const std::vector <geom::Coordinate>& coords = *coords_ptr;

    if(cloud->hasChannel(10) && cloud->hasChannel(11) && cloud->hasChannel(12))
    {
        std::cout << "we have color" << std::endl;

        colors = true;
    }

    std::vector<geom::Coordinate> c;
    std::vector<double> r;
    std::vector<double> g;
    std::vector<double> b;

    double minX = coords[0].x;
    double minZ = coords[0].z;

    double maxX = minX;
    double maxZ = minZ;

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

    int intersections = 0;query::PolygonQuery::Ptr query

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
                //std::cout << "In" << std::endl;
                c.push_back(cloud_coords[j]);
                if(colors == true)
                {
                    //std::cout << cloud->getChannel(10)[j] << std::endl;
                    r.emplace_back(cloud->getChannel(10)[j]);
                    //std::cout << cloud->getChannel(11)[j] << std::endl;
                    //std::cout << cloud->getChannel(12)[j] << std::endl;
                    g.emplace_back(cloud->getChannel(11)[j]);
                    b.emplace_back(cloud->getChannel(12)[j]);
                }

            }
            //std::cout << intersections << std::endl;
            intersections = 0;
        }

    }

    entity::PointCloud::Ptr entity = entity::PointCloud::Ptr(new entity::PointCloud());

    entity->setCoordinates(c);
    if(colors == true)
    {
        std::cout << "Set Colors" << std::endl;
        entity->setChannel(10, r);
        entity->setChannel(11, g);
        entity->setChannel(12, b);
    }

    query->results = entity;
    // this is a really bad running time, isnt it? .. O(NumberOfClouds) * O(NumberOfPointsInEachCloud) * O(NumberOfVerticesInPolygon) * O(calculating) ... :(

}
*/

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

bool PointCloudModule::intersect(std::vector<geom::Coordinate> c, std::vector <geom::Coordinate> p)
{
    unsigned int i;
    double cminX = c[0].x;
    double cminZ = c[0].z;

    double cmaxX = cminX;
    double cmaxZ = cminZ;

    if(c.empty() || p.empty())
    {
        std::cout << "empty" << std::endl;

        return false;
    }

    for(i = 1; i < c.size() - 1; i++)
    {getCoordinate(
        if(c[i].x < cminX)
            cminX = c[i].x;
        if(c[i].z < cminZ)
            cminZ = c[i].z;
        if(c[i].x > cmaxX)
            cmaxX = c[i].x;
        if(c[i].z > cmaxZ)
            cmaxZ = c[i].z;
    }


    double pminX = c[0].x;
    double pminZ = c[0].z;

    double pmaxX = pminX;
    double pmaxZ = pminZ;

    for(i = 1; i < p.size() - 1; i++)
    {
        if(p[i].x < pminX)
            pminX = p[i].x;
        if(p[i].z < pminZ)
            pminZ = p[i].z;
        if(p[i].x > pmaxX)
            pmaxX = p[i].x;
        if(p[i].z > pmaxZ)
            pmaxZ = p[i].z;
    }

    std::cout << "Bounds PC: x_min = " << cminX << "; x_max = " << cmaxX << "; z_min = " << cminZ << "; z_max = " << cmaxZ << std::endl;
    std::cout << "Bounds PO: x_min = " << pminX << "; x_max = " << pmaxX << "; z_min = " << pminZ << "; z_max = " << pmaxZ << std::endl;

    if((pminX <= cmaxX || pmaxX >= cminX) && (pminZ <= cmaxZ || pmaxZ >= cminZ))
        return true;

    return false;
}

}}
