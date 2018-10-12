#ifndef SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_
#define SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_

#include <sempr/entity/spatial/AbstractPointCloud.hpp>

#include <sempr/entity/spatial/Geometry.hpp>
#include <geos/geom/MultiPoint.h>

#include <stdexcept>
#include <vector>
#include <map>

#include <type_traits>

#include <boost/variant.hpp>


namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
 * The CoordinatePoint is a Wrapper of a geos::geom Coordinate to an AbstractPoint of the AbstractPointCloud.
 * It allows a implicit type casting from a coordinate.
 */
class CoordinatePoint : public AbstractPoint<double>
{
private:
    geom::Coordinate& coord;

public:
    CoordinatePoint(geom::Coordinate& coord) : coord(coord) {}; //allows implicit type cast
    CoordinatePoint(const geom::Coordinate& coord) : coord(const_cast<geom::Coordinate&>(coord)) {}; //allows implicit type cast

    inline double& x() override {return coord.x;};
    inline double& y() override {return coord.y;};
    inline double& z() override {return coord.z;};

    const double& operator[](std::size_t idx) const override
    {
        if (idx == 0)
            return coord.x;
        else if (idx == 1)
            return coord.y;
        else if (idx == 2)
            return coord.z;
        else
            throw std::out_of_range(""); // Out of boundary!

        return coord.z;
    }
};



/**
 * A channel will store additional information for every point of a PointCloud.
 * The channel information could have different types e.g. unsigned char for red green and blue or float for the intensity.
 * 
 * The size of a channel have to equal the size of the depending point cloud!
 * 
 * Note: It has no pragma as db value to force odb to use the trait.
 */
template<typename T>
class Channel : public AbstractChannel<T>
{
public:
    Channel() {};
    Channel(const std::vector<T>& channel) : channel_(channel) {}; //allows implicit type cast

    // Pre init the channel with the size e.g. of a the point cloud
    Channel(std::size_t size) : channel_(std::vector<T>(size)) {};

    inline std::size_t size() const override {return channel_.size();};
    inline T& operator[](std::size_t idx) override {return channel_[idx];};
    inline const T& operator[](std::size_t idx) const override {return channel_[idx];};

    inline void add(const T& value) { channel_.push_back(value); }; 

    inline T* data() { return channel_.data(); };

    inline void resize(std::size_t size) { channel_.resize(size); };

    friend std::ostream& operator<< (std::ostream& stream, const Channel<T>& channel)
    {
        for (std::size_t i=0; i < channel.size(); i++)
        {
            stream << channel[i];

            if(i+1 != channel.size())
                stream << " ";
        }

        return stream;
    }

private:
    friend class odb::access;

    std::vector<T> channel_;
};

/**
 * This variant allows to store channels of different "simple" types in one list of channels.
 * 
 * Based on compiler conflicts a boolean channel is not supported. Please use a int8 value otherwise. (A boolean also need a at least 8bit to be stored!)
 * 
 * Note: The boost variant could be replaced with the std::variant in a future C++17 build.
 */
typedef boost::variant< Channel<int8_t>,    // shall be used for boolean values
                        Channel<int16_t>, 
                        Channel<int32_t>, 
                        Channel<int64_t>, 
                        Channel<uint8_t>, 
                        Channel<uint16_t>, 
                        Channel<uint32_t>, 
                        Channel<uint64_t>, 
                        Channel<float>, 
                        Channel<double>      > ChannelVariant;


/**
 * The PointCloud Entity allows wo store a set of points with an optional list of channels for additional information of every point.
 * All points and channels will be stored as binary blob.
 * 
 * The PointCloud is build like a geometry object but it still is no geometry by its one. It only contains a geometry multi point object.
 * 
 * Note: Since it is currently not possible to have multiple inheritance for entities, the PointCloud class will not derive from the AbstractPointCloud but fullfill all there methodes.
 */
#pragma db object
class PointCloud : public Geometry, public AbstractPointCloud<double> //ToDo Change to Geometry!
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

    /**
     * Check if the PointCloud holds a channel of the given channel and type of data.
     */
    template<typename T>
    bool checkChannel(int type) const
    {
        if (hasChannel(type))
        {
            // this could be replaced with std::holds_alternative in C++17
            try
            {
                boost::get< Channel<T> >(channels_.at(type));   // will throw on type missmatch!
                return true;
            }
            catch (const boost::exception& ex)
            {
                return false;
            }
        }

        return false;
    }

    template<typename T>
    void setChannel(int type, const Channel<T>& channel)
    {
        if(channel.size() != size())
            throw std::exception(); // no equal size of points and channel information

        channels_[type] = channel;
    }

    template<typename T>
    AbstractChannel<T>& getChannel(int type)
    {
        if (!hasChannel(type))
            throw std::out_of_range("Channel " + std::to_string(type) + " not existing.");

        return boost::get< Channel<T> >(channels_[type]);
    }
    
    template<typename T>
    const AbstractChannel<T>& getChannel(int type) const
    {
        if (!hasChannel(type))
            throw std::out_of_range("Channel " + std::to_string(type) + " not existing.");

        return boost::get< Channel<T> >(channels_.at(type));
    }

    virtual void getChannel(int type, AbstractChannel<int8_t>&& channel) override { channel = getChannel<int8_t>(type); };
    
    virtual void getChannel(int type, AbstractChannel<int16_t>&& channel) override { channel = getChannel<int16_t>(type); };
    virtual void getChannel(int type, AbstractChannel<int32_t>&& channel) override { channel = getChannel<int32_t>(type);};
    virtual void getChannel(int type, AbstractChannel<int64_t>&& channel) override { channel = getChannel<int64_t>(type); };

    virtual void getChannel(int type, AbstractChannel<uint8_t>&& channel) override { channel = getChannel<uint8_t>(type); };
    virtual void getChannel(int type, AbstractChannel<uint16_t>&& channel) override { channel = getChannel<uint16_t>(type); };
    virtual void getChannel(int type, AbstractChannel<uint32_t>&& channel) override { channel = getChannel<uint32_t>(type); };
    virtual void getChannel(int type, AbstractChannel<uint64_t>&& channel) override { channel = getChannel<uint64_t>(type); };

    virtual void getChannel(int type, AbstractChannel<float>&& channel) override { channel = getChannel<float>(type); };
    virtual void getChannel(int type, AbstractChannel<double>&& channel) override { channel = getChannel<double>(type); };

    virtual std::size_t size() const
    {
        // each point is a geometry so the num of geometries shall be equal to the num of points!
        return getGeometry()->getNumGeometries(); 
    }

    virtual const AbstractPoint<double>::Ptr operator[](std::size_t idx) const
    {
        // Note: this is an ineffective way to it because for each call it will create a copy of the coordinate on the heap!
        // But this version is safe! Dynamicly type remapping could easily cause undefined behaviour.
        return std::make_shared<CoordinatePoint>(*getGeometry()->getGeometryN(idx)->getCoordinate());
    }

    const CoordinatePoint at(std::size_t idx) const
    {
        return CoordinatePoint(*getGeometry()->getGeometryN(idx)->getCoordinate());
    }

    virtual AbstractPoint<double>::Ptr operator[](std::size_t idx)
    {
        return std::make_shared<CoordinatePoint>(at(idx));
    }

    geom::Coordinate& at(std::size_t idx)
    {
        return *const_cast<geom::Coordinate*>(getGeometry()->getGeometryN(idx)->getCoordinate());
    }

    const geom::MultiPoint* getGeometry() const;

    void setGeometry(geom::MultiPoint* geometry);

    void setPoints(const std::vector<geom::Coordinate>& coordinates);

    PointCloud::Ptr clone() const;

protected:
    geom::MultiPoint* getGeometryMut();

private:
    friend class odb::access;

    #pragma db type("BLOB")
    geom::MultiPoint* geometry_;

    // odb will not solve it with a map with an self defined struct.
    // so this a workaround to tell odb how to handle this map.
    #pragma db table("PointCloud_channels")      \
             id_column("object_id")   \
             key_type("INT") \
             key_column("type")        \
             value_type("BLOB")     \
             value_column("channel")
    std::map< int, ChannelVariant > channels_;

    virtual PointCloud* raw_clone() const;
};

    
}}

#endif /* end of include guard: SEMPR_ENTITY_SPATIAL_POINTCLOUD_HPP_ */
