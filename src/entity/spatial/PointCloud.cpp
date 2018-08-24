#ifndef SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_
#define SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_

#include <sempr/entity/spatial/AbstractPointCloud.hpp>

#include <sempr/entity/spatial/MultiPoint.hpp>
#include <geos/geom/MultiPoint.h>

#include <stdexcept>
#include <vector>
#include <map>

#include <type_traits>

#include <boost/variant.hpp>

namespace sempr { namespace entity {

namespace geom = geos::geom;

// Wrapper for GOES::GEOM Coordinate to AbstractPoint
class CoordinatePoint : public AbstractPoint<double>, public geom::Coordinate
{
public:
    CoordinatePoint(const geom::Coordinate& coord) : geom::Coordinate(coord.x, coord.y, coord.z) {}; //allows implicit type cast

    inline double getX() override {return x;};
    inline double getY() override {return y;};
    inline double getZ() override {return z;};

    const double& operator[](std::size_t idx) const override
    {
        if (idx == 0)
            return x;
        else if (idx == 1)
            return y;
        else if (idx == 2)
            return z;
        else
            throw std::out_of_range(""); // Out of boundary!

        return z;
    }
};


// No pragma db value to force odb to use the trait.
template<typename T>
class Channel : public AbstractChannel<T>
{
public:
    Channel() {};
    Channel(const std::vector<T>& channel) : channel_(channel) {}; //allows impecit type cast

    // Pre init the channel with the size e.g. of a the point cloud
    Channel(std::size_t size) : channel_(std::vector<T>(size)) {};

    inline std::size_t size() const override {return channel_.size();};
    inline T& operator[](std::size_t idx) override {return channel_[idx];};
    inline const T& operator[](std::size_t idx) const override {return channel_[idx];};

private:
    friend class odb::access;

    std::vector<T> channel_;
};

// Could be changed to std::variant in C++17
typedef boost::variant< Channel<int8_t>,
                        Channel<int16_t>,
                        Channel<int32_t>,
                        Channel<int64_t>,
                        Channel<uint8_t>,
                        Channel<uint16_t>,
                        Channel<uint32_t>,
                        Channel<uint64_t>,
                        Channel<float>,
                        Channel<double>      > ChannelVariant;


#pragma db object
/**
 * @brief The PointCloud class is a Entity that represents a Pointcloud
 * Currently stored as text - shall be a bin blob
 */
class PointCloud : public MultiPoint /*,public AbstractPointCloud<double> */
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<PointCloud>;

    PointCloud();
    PointCloud(const sempr::core::IDGenBase*);

    virtual ~PointCloud();

    virtual bool hasChannel(int type) const
    {
        auto it = channels_.find(type);

        return it != channels_.end();
    }

    template<typename T>
    //typename std::enable_if<std::is_fundamental<T>::value>
    void setChannel(int type, const Channel<T>& channel)
    {
        if(channel.size() != size())
            throw std::exception(); // no equial size of points and channel information

        channels_[type] = channel;
    }

    template<typename T>
    //typename std::enable_if<std::is_fundamental<T>::value>
    AbstractChannel<T>& getChannel(int type)
    {
        if (!hasChannel(type))
            throw std::out_of_range("Channel " + std::to_string(type) + " not existing.");

        return boost::get< Channel<T> >(channels_[type]);
    }

    template<typename T>
    //typename std::enable_if<std::is_fundamental<T>::value>
    const AbstractChannel<T>& getChannel(int type) const
    {
        if (!hasChannel(type))
            throw std::out_of_range("Channel " + std::to_string(type) + " not existing.");

        return boost::get< Channel<T> >(channels_.at(type));
    }

    template<typename T>
    //typename std::enable_if<std::is_fundamental<T>::value>
    void getChannel(int type, AbstractChannel<T>& channel) const   // will this work and override the base class methods?
    {
        if (!hasChannel(type))
            throw std::out_of_range("Channel " + std::to_string(type) + " not existing.");

        channel = boost::get< Channel<T> >(channels_.at(type));
    }

    virtual std::size_t size() const
    {
        // each point is a geometry so the num of geometries shall be equal to the num of points!
        return getGeometry()->getNumGeometries();
    }

    virtual const AbstractPoint<double>::Ptr operator[](std::size_t idx) const
    {
        // Note: this is an ineffective way to it because for each call it will create a copy of the coordinate on the heap!
        // But this version is safe!
        return std::make_shared<CoordinatePoint>(*getGeometry()->getGeometryN(idx)->getCoordinate());
    }

    const CoordinatePoint at(std::size_t idx) const
    {
        return CoordinatePoint(*getGeometry()->getGeometryN(idx)->getCoordinate());
    }

    geom::Coordinate& at(std::size_t idx)
    {
        return *const_cast<geom::Coordinate*>(getGeometry()->getGeometryN(idx)->getCoordinate());
    }

private:
    friend class odb::access;

    #pragma db table("PointCloud_channels")      \
             id_column("object_id")   \
             key_type("INT") \
             key_column("type")        \
             value_type("TEXT")     \
             value_column("channel")
    std::map< int, ChannelVariant > channels_;     //workaround because odb will not solve std container in std container.
};


}}

#endif /* end of include guard: SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_ */
