#include <sempr/processing/CalculatePointsModule.hpp>
#include <limits>
#include <sempr/entity/spatial/MultiPoint.hpp>
#include <sempr/entity/spatial/PointCloud.hpp>

namespace sempr { namespace processing {

CalculatePointsModule::CalculatePointsModule()
{
    // nothing to do?
}

CalculatePointsModule::~CalculatePointsModule()
{
    // nothing to do?
}

std::string CalculatePointsModule::type() const {
    return "CalculatePointsModule";
}

void CalculatePointsModule::process(query::CalculatePointsQuery::Ptr query)
{

    query::ObjectQuery<entity::PointCloud>::Ptr cloudQuery(new query::ObjectQuery<entity::PointCloud>() );
    ask(cloudQuery);

    for (auto const& cloud : cloudQuery->results)
    {
        cloud->loaded();
    }

    auto spatialQuery = query::SpatialIndexQuery::intersectsBoxOf(query->entity());
    auto coords = *((spatialQuery->refGeo())->getGeometry()->getCoordinates()->toVector());

    // TODO: Use this coords for the calculatePoints() function
    coords[0].z = coords[1].z = coords[4].z = coords[5].z = -DBL_MAX;
    coords[2].z = coords[3].z = coords[6].z = coords[7].z = DBL_MAX;

    std::static_pointer_cast<entity::MultiPoint>(spatialQuery->refGeo())->setCoordinates(coords);
    ask(spatialQuery);
    entity::PointCloud::Ptr calculatedPointCloud = entity::PointCloud::Ptr(new entity::PointCloud());

    //ToDo the intersection calculation could be done by a spatialQuery later on!

    std::vector<geom::Coordinate> points;
    std::vector<uint8_t> colorR, colorG, colorB;
    bool hasColors;
    for(auto& cloudIntersection : spatialQuery->results)
    {
        if(cloudIntersection->discriminator() == "sempr::entity::PointCloud")
        {
            entity::PointCloud::Ptr intersectingCloud = std::static_pointer_cast<entity::PointCloud>(cloudIntersection);
            calculatePoints(intersectingCloud, query, points, colorR, colorG, colorB, hasColors);
        }
    }
    calculatedPointCloud->setPoints(points);

    if(hasColors)
    {

        calculatedPointCloud->setChannel(entity::ChannelType::COLOR_R, entity::Channel<uint8_t>(colorR));
        calculatedPointCloud->setChannel(entity::ChannelType::COLOR_G, entity::Channel<uint8_t>(colorG));
        calculatedPointCloud->setChannel(entity::ChannelType::COLOR_B, entity::Channel<uint8_t>(colorB));
    }
    query->results = calculatedPointCloud;
}

void CalculatePointsModule::calculatePoints(const entity::PointCloud::Ptr intersectingCloud, query::CalculatePointsQuery::Ptr query, std::vector<geos::geom::Coordinate>& p, std::vector<uint8_t>& r, std::vector<uint8_t>& g, std::vector<uint8_t>& b, bool& colors)
{
    double low = query->minHeight();
    double high = query->maxHeight();

    const std::vector <geom::Coordinate>* coords_ptr = query->entity()->getGeometry()->getCoordinates()->toVector();
    const std::vector <geom::Coordinate>& poly = *coords_ptr;

    colors = intersectingCloud->hasChannel(entity::ChannelType::COLOR_R) && intersectingCloud->hasChannel(entity::ChannelType::COLOR_G) && intersectingCloud->hasChannel(entity::ChannelType::COLOR_B);

    //build min/max

    double minX = poly[0].x;
    double minY = poly[0].y;

    double maxX = minX;
    double maxY = minY;

    for(std::size_t i = 1; i < poly.size() - 1; i++)
    {
        if(poly[i].x < minX)
            minX = poly[i].x;
        if(poly[i].y < minY)
            minY = poly[i].y;
        if(poly[i].x > maxX)
            maxX = poly[i].x;
        if(poly[i].y > maxY)
            maxY = poly[i].y;
    }


    //std::cout << "MinX:  "<< minX << "; MaxX: " << maxX << "; Min>: " << minY << "; MaxY: " << maxY << std::endl;

    const std::vector <geom::Coordinate>* cloud_coords_ptr = intersectingCloud->getGeometry()->getCoordinates()->toVector();
    const std::vector <geom::Coordinate>& cloud_coords = *cloud_coords_ptr;

    for (std::size_t j = 0; j < cloud_coords.size(); j++)
    {
        auto& coord = cloud_coords[j];

        if (coord.x > minX && coord.x && maxX && coord.y > minY && coord.y < maxY && coord.z >= low && coord.z <= high)
        {

            int intersections = 0;
            geom::Coordinate outerPoint(minX - 10, minY - 10, 0);
            for(std::size_t i = 1; i < poly.size(); i++)
            {
                intersections += checkIntersection(poly[i - 1], poly[i], outerPoint, cloud_coords[j]);
            }

            if((intersections & 0x1) == 1)
            {
                p.push_back(cloud_coords[j]);
                if(colors == true)
                {
                    r.emplace_back(intersectingCloud->getChannelUInt8(entity::ChannelType::COLOR_R)[j]);
                    g.emplace_back(intersectingCloud->getChannelUInt8(entity::ChannelType::COLOR_G)[j]);
                    b.emplace_back(intersectingCloud->getChannelUInt8(entity::ChannelType::COLOR_B)[j]);
                }

            }

        }
    }
}

int CalculatePointsModule::checkIntersection(geom::Coordinate v11, geom::Coordinate v12, geom::Coordinate v21, geom::Coordinate v22)
{
    double a1, a2;
    double b1, b2;
    double c1, c2;
    double d1, d2;

    a1 = v12.y - v11.y;
    b1 = v11.x - v12.x;
    c1 = (v12.x * v11.y) - (v11.x * v12.y);

    d1 = (a1 * v21.x) + (b1 * v21.y) + c1;
    d2 = (a1 * v22.x) + (b1 * v22.y) + c1;

    if ((d1 > 0 && d2 > 0) || (d1 < 0 && d2 < 0))
    {
        return 0;
    }

    a2 = v22.y - v21.y;
    b2 = v21.x - v22.x;
    c2 = (v22.x * v21.y) - (v21.x * v22.y);

    d1 = (a2 * v11.x) + (b2 * v11.y) + c2;
    d2 = (a2 * v12.x) + (b2 * v12.y) + c2;

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
