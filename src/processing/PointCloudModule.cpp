#include <sempr/processing/PointCloudModule.hpp>
#include <limits>
#include <sempr/entity/spatial/MultiPoint.hpp>

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
    m_c.clear();
    m_r.clear();
    m_g.clear();
    m_b.clear();
    m_colors = false;

    query::ObjectQuery<entity::PointCloud>::Ptr clouds(new query::ObjectQuery<entity::PointCloud>() );


    ask(clouds);
    int i = 0;

    for (auto const& cloud : clouds->results)
    {
        cloud->loaded();
    }


    auto spatial = query::SpatialIndexQuery::intersectsBoxOf(query->entity());
    auto coords = *(spatial->refGeo()->getGeometry()->getCoordinates()->toVector());

    //TODO: Use this coords for the calculatePoints() function
    coords[0].y = -DBL_MAX;
    coords[1].y = -DBL_MAX;
    coords[2].y = DBL_MAX;
    coords[3].y = DBL_MAX;
    coords[4].y = -DBL_MAX;
    coords[5].y = -DBL_MAX;
    coords[6].y = DBL_MAX;
    coords[7].y = DBL_MAX;

    std::static_pointer_cast<entity::MultiPoint>(spatial->refGeo())->setCoordinates(coords);

    ask(spatial);

    entity::PointCloud::Ptr entity = entity::PointCloud::Ptr(new entity::PointCloud());

    for(auto& c : spatial->results)
    {
        if(c->discriminator() == "sempr::entity::PointCloud")
        {
            entity::PointCloud::Ptr cloud = std::static_pointer_cast<entity::PointCloud>(c);

            calculatePoints(cloud, query);
        }
    }

    entity->setCoordinates(m_c);
    if(m_colors == true)
    {
        entity->setChannel(10, m_r);
        entity->setChannel(11, m_g);
        entity->setChannel(12, m_b);
    }

    query->results = entity;
}

void PointCloudModule::calculatePoints(const entity::PointCloud::Ptr cloud, query::PolygonQuery::Ptr query)
{
    unsigned int i, j;

    double low = query->low();
    double high = query->high();

    const std::vector <geom::Coordinate>* coords_ptr = query->entity()->getGeometry()->getCoordinates()->toVector();
    const std::vector <geom::Coordinate>& coords = *coords_ptr;
    
    if(cloud->hasChannel(10) && cloud->hasChannel(11) && cloud->hasChannel(12))
    {
        m_colors = true;
    }
    else
    {
        m_colors = false;
    }

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
                    m_c.push_back(cloud_coords[j]);
                    if(m_colors == true)
                    {
                        m_r.emplace_back(cloud->getChannel(10)[j]);
                        m_g.emplace_back(cloud->getChannel(11)[j]);
                        m_b.emplace_back(cloud->getChannel(12)[j]);
                    }
                }
            }
            intersections = 0;
        }
    }
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
