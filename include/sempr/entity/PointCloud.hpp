#ifndef SEMPR_ENTITY_POINTCLOUD_HPP_
#define SEMPR_ENTITY_POINTCLOUD_HPP_

#include <sempr/storage/History.hpp>
#include <sempr/entity/Entity.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
#include <memory>
#include <vector>
#include <algorithm>

#include <type_traits>

namespace sempr { namespace entity {
    
#pragma db object
/**
 * @brief The PointCloud class is a Entity that represents a Pointcloud
 */
class PointCloud : public Entity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<PointCloud>;

    PointCloud();
    PointCloud(const sempr::core::IDGenBase*);

    struct Point
    {
        double x, y, z;
        unsigned char r, g, b;
        // more for hyperspectral, but how?
    };

#pragma db value
    struct BoundingBox
    {
        double minX, maxX;
        double minY, maxY;
        double minZ, maxZ;
    };


    uint64_t size() const { return m_points.size(); }

    BoundingBox& bounds() { return m_bounds; }
    std::vector<Point>& points() { return m_points; }
    // TODO: error-handling? i out of bounds?
    Point& point(int i) { return m_points[i]; }

    void setPointsWithColor(const std::vector<double> &points, const std::vector<unsigned char> &colors);
    void setPoints(const std::vector<Point>& points) { m_points = points; calculateBounds(); changed(); }

    void addPoint(const Point& p) { checkBounds(p); m_points.push_back(p); changed(); }
    void removePoint(uint64_t i);

    // these are no save operations.
    // are these necessary?
    double x(int i) { return m_points[i].x; }
    double y(int i) { return m_points[i].y; }
    double z(int i) { return m_points[i].z; }
    unsigned char r(int i) { return m_points[i].r; }
    unsigned char g(int i) { return m_points[i].g; }
    unsigned char b(int i) { return m_points[i].b; }

    virtual ~PointCloud() {}

protected:
    friend class odb::access;

    inline void checkBounds(const Point& point);
    void calculateBounds();

#pragma db type("BLOB")
    // @Blob:   The danger of this kind of operation is that the binary representation
    //          of the resulting array will not be portable (e.g., one applications is
    //          big-endian while another is little-endian; and in case of doubles, even
    //          ABIs with the same endian-ness can have different representations)
    // From:    https://www.codesynthesis.com/pipermail/odb-users/2012-January/000441.html
    std::vector<Point> m_points;

    BoundingBox m_bounds;
};

// enable history:
typedef storage::History<PointCloud::Ptr> PointCloudHistory;
#pragma db value(PointCloudHistory)
    
}}

#endif /* end of include guard: SEMPR_ENTITY_POINTCLOUD_HPP_ */
