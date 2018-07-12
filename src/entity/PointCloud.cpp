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
    m_bounds.minX = 0.0;
    m_bounds.maxX = 0.0;
    m_bounds.minY = 0.0;
    m_bounds.maxY = 0.0;
    m_bounds.minZ = 0.0;
    m_bounds.maxZ = 0.0;
}

/*
int PointCloud::p(uint64_t i, std::vector<double>& p)
{
    if (p.size() != 3) // hmm ...
        p.resize(3);
    if( (this->x(i, p[0]) == 0) && (this->y(i, p[1]) == 0) && (this->z(i, p[2]) == 0))
        return 0;
    return -1;
}

int PointCloud::w(uint64_t i, double& w, uint64_t j)
{
    if (i < this->size())
    {
        w = m_points.get()->points[(i * 3) + j];
        return 0;
    }
    return -1;
}

int PointCloud::c(uint64_t i, std::vector<unsigned char>& c)
{
    if (c.size() != 3)
        c.resize(3);
    if( (this->r(i, c[0]) == 0) && (this->g(i, c[1]) == 0) && (this->b(i, c[2]) == 0))
        return 0;
    return -1;
}

int PointCloud::a(uint64_t i, unsigned char& a, uint64_t j)
{
    // check for m_colors.emtpy() ?
    // or check for i < m_colors.capacity() ?
    if (i < this->size())
    {
        a = m_points.get()->colors[(i * 3) + j];
        return 0;
    }
    return -1;
}
*/

void PointCloud::calculateBounds()
{
    for(auto const& point : m_points)
    {
        if(point.x < m_bounds.minX) m_bounds.minX = point.x;
        if(point.x > m_bounds.maxX) m_bounds.maxX = point.x;
        if(point.y < m_bounds.minY) m_bounds.minY = point.y;
        if(point.y > m_bounds.maxY) m_bounds.maxY = point.y;
        if(point.z < m_bounds.minZ) m_bounds.minZ = point.z;
        if(point.z > m_bounds.maxZ) m_bounds.maxZ = point.z;
    }
}

void PointCloud::setPointsWithColor(std::vector<double> const& points,
                                    std::vector<unsigned char> const& colors,
                                    uint64_t n)
{
    uint64_t k = 0;
    std::cout << "in" << std::endl;

    //static_assert(points.size() == (3 * n), "Too much or not enough point values");
    //static_assert(colors.size() == (3 * n), "Too much or not enough color values");

    // does it make sence to check if there are allready points?
    this->m_points = std::vector<Point>(n);

    //for(std::vector<Point>::iterator it = m_points.begin(); it != )
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

    // we need to trust the input? :(
    // maybe think about some saver method to get the points (and the number)
    //this->points(points);
    //this->colors(colors);
    calculateBounds();

    changed();

    std::cout << "process finished" << std::endl;
    std::cout << "Testpoint: " << this->m_points[n/3].x << std::endl;

}

}}
