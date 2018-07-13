#include <sempr/entity/PointCloud.hpp>
#include <PointCloud_odb.h>

#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(PointCloud)

PointCloud::PointCloud() : PointCloud(new sempr::core::IDGen<PointCloud>())
{
}

PointCloud::PointCloud(const sempr::core::IDGenBase* idgen) : sempr::entity::Entity(idgen)
{
    // initialize the bounds with zero should be okay ...
    m_bounds.minX = 0.0;
    m_bounds.maxX = 0.0;
    m_bounds.minY = 0.0;
    m_bounds.maxY = 0.0;
    m_bounds.minZ = 0.0;
    m_bounds.maxZ = 0.0;
}

void PointCloud::calculateBounds()
{
    // make sure, that we get the right bounds
    m_bounds.minX = m_bounds.maxX = m_points[0].x;
    m_bounds.minY = m_bounds.maxY = m_points[0].y;
    m_bounds.minZ = m_bounds.maxZ = m_points[0].z;

    for(auto const& point : m_points)
    {
        checkBounds(point);
    }
}

inline void PointCloud::checkBounds(const Point &point) {
    if(point.x < m_bounds.minX) m_bounds.minX = point.x;
    if(point.x > m_bounds.maxX) m_bounds.maxX = point.x;
    if(point.y < m_bounds.minY) m_bounds.minY = point.y;
    if(point.y > m_bounds.maxY) m_bounds.maxY = point.y;
    if(point.z < m_bounds.minZ) m_bounds.minZ = point.z;
    if(point.z > m_bounds.maxZ) m_bounds.maxZ = point.z;
}

void PointCloud::removePoint(uint64_t i)
{
    Point& p = m_points[i];
    if(p.x == m_bounds.minX || p.x == m_bounds.maxX ||
       p.y == m_bounds.minY || p.y == m_bounds.maxY ||
       p.z == m_bounds.minZ || p.z == m_bounds.maxZ)
    {
        calculateBounds();
    }

    m_points.erase(m_points.begin() + i);
    changed();
}

void PointCloud::setPointsWithColor(std::vector<double> const& points,
                                    std::vector<unsigned char> const& colors)
{
    uint64_t k = 0;

    // does it make sence to check if there are allready points?
    m_points = std::vector<Point>(points.size() / 3);

    for(auto & point : m_points)
    {
        // calculate the points here?!
        point.x = points[k];
        point.r = colors[k++];
        point.y = points[k];
        point.g = colors[k++];
        point.z = points[k];
        point.b = colors[k++];
    }

    calculateBounds();
    changed();
}

}}
