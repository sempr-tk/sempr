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
    CoordinatePoint(const geom::Coordinate& coord) : coord_(coord) {}; //allows implicit type cast

    inline double getX() override {return coord_.x;};
    inline double getY() override {return coord_.y;};
    inline double getZ() override {return coord_.z;};

    const double& operator[](std::size_t idx) const override;

private:
    const geom::Coordinate& coord_;
};


// No pragma db value to force odb to use the trait.
class Channel : public AbstractChannel
{
public:
    Channel() {};
    Channel(const std::vector<double>& channel) : channel_(channel) {}; //allows impecit type cast

    // Pre init the channel with the size e.g. of a the point cloud
    Channel(std::size_t size) : channel_(std::vector<double>(size)) {};

    inline std::size_t size() const override {return channel_.size();};
    inline double& operator[](std::size_t idx) override {return channel_[idx];};
    inline const double& operator[](std::size_t idx) const override {return channel_[idx];};

private:
    friend class odb::access;

    std::vector<double> channel_;
};


#pragma db object
/**
 * @brief The PointCloud class is a Entity that represents a Pointcloud
 */
class PointCloud : public MultiPoint /*, AbstractPointCloud*/
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<PointCloud>;

    PointCloud();
    PointCloud(const sempr::core::IDGenBase*);

    virtual ~PointCloud();

    virtual bool hasChannel(int type) const;

    void setChannel(int type, const Channel& channel);

    virtual AbstractChannel& getChannel(int type);
    virtual const AbstractChannel& getChannel(int type) const;

    virtual std::size_t size() const;

    virtual const AbstractPoint& operator[](std::size_t idx) const;

private:
    friend class odb::access;
    
    #pragma db table("PointCloud_channels")      \
             id_column("object_id")   \
             key_type("INT") \
             key_column("type")        \
             value_type("TEXT")     \
             value_column("channel")
    std::map< int, Channel > channels_;     //workaround because odb will not solve std container in std container.
};

    
}}

#endif /* end of include guard: SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_ */