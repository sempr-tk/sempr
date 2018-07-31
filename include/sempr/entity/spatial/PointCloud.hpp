#ifndef SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_
#define SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_

#include <sempr/storage/History.hpp>

#include <sempr/entity/spatial/MultiPoint.hpp>
#include <geos/geom/MultiPoint.h>

#include <vector>

#include <type_traits>

namespace sempr { namespace entity {

namespace geom = geos::geom;
    
#pragma db object
/**
 * @brief The PointCloud class is a Entity that represents a Pointcloud
 */
class PointCloud : public MultiPoint
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<PointCloud>;

    PointCloud();
    PointCloud(const sempr::core::IDGenBase*);

    ~PointCloud() {}

    #pragma db value
    struct Color
    {
        unsigned char r, g, b;
    };

    #pragma db value
    struct PointData
    {
        struct Color c;
    };

    uint64_t size() const { return geometry()->getNumPoints(); }
    // TODO How to store the colors?

    //std::vector<geom::Coordinate>& points() { return geometry_->getCoordinates(); }
    geom::CoordinateSequence *points() { return geometry()->getCoordinates(); }
    // TODO: error-handling? i out of bounds?
    geom::Geometry const *point(int i) { return geometry()->getGeometryN(i); }           // beware of the CONST! point.

    void setCoordinatesWithColor(const std::vector<geom::Coordinate> &coordinates, const std::vector<unsigned char> &colors);
    //void setPoints(const std::vector<Point>& points) { m_points = points; calculateBounds(); changed(); }

    // This seems to be complicated with geos? Create new geometry if we want to add/remove
    //void addPoint(const Point& p) { checkBounds(p); m_points.push_back(p); changed(); }
    //void removePoint(uint64_t i);


protected:
    friend class odb::access;
};

// enable history:
typedef storage::History<PointCloud::Ptr> PointCloudHistory;
#pragma db value(PointCloudHistory)
    
}}

#endif /* end of include guard: SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_ */
