#ifndef SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_
#define SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_

#include <sempr/entity/spatial/AbstractPointCloud.hpp>

#include <sempr/entity/spatial/MultiPoint.hpp>
#include <geos/geom/MultiPoint.h>

#include <stdexcept>
#include <vector>
#include <map>

#include <type_traits>

namespace sempr { namespace entity {

namespace geom = geos::geom;

// Wrapper for GOES::GEOM Coordinate to AbstractPoint
class CoordinatePoint : public AbstractPoint
{
public:
    CoordinatePoint(const geom::Coordinate& coord); //allows implicit type cast

    inline double getX() override {return coord_.x;};
    inline double getY() override {return coord_.y;};
    inline double getZ() override {return coord_.z;};

    //double& operator[](std::size_t idx) override;
    const double& operator[](std::size_t idx) const override;

private:
    const geom::Coordinate& coord_;
};


#pragma db value
struct Channel
{
    Channel() {};
    Channel(const std::vector<double>& channel) : channel_(channel) {};
    std::vector<double> channel_;
};


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

    virtual ~PointCloud();

    virtual bool hasChannel(int type) const;

    void setChannel(int type, const std::vector<double>& channel);

    virtual std::vector<double>& getChannel(int type);
    //virtual const std::vector<double>& getChannel(int type) const;

    virtual std::size_t size() const;

    virtual const CoordinatePoint operator[](std::size_t idx) const;

private:
    friend class odb::access;

    //std::map< int, std::vector<double> > channels_;
    std::map< int, Channel > channels_;     //workaround because odb will not solve std container in std container.
};

    
}}

#endif /* end of include guard: SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_ */
