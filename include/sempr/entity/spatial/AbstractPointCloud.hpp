#ifndef SEMPR_ENTITY_SPATIAL_ABSTRACTPOINTCLOUD_HPP_
#define SEMPR_ENTITY_SPATIAL_ABSTRACTPOINTCLOUD_HPP_

#include <vector>
#include <memory>
#include <functional>

namespace sempr { namespace entity {


enum class ChannelType
{
    N_X = 0,    // Normale in X
    N_Y = 1,    // Normale in Y
    N_Z = 2,    // Normale in Z
    V = 4,      // Velocity in normal direction
    I = 5,      // Intensity
    R = 10,     // Red 
    G = 11,     // Green
    B = 12,     // Blue
    
    H_0 = 100   // Hyperspectral Channel 0
    // e.g. H_0 + 10 = Hyperspectral Channel 10
};

class AbstractPoint
{
public:
    virtual double getX() = 0;
    virtual double getY() = 0;
    virtual double getZ() = 0;

    //virtual double& operator[](std::size_t idx) = 0;
    virtual const double& operator[](std::size_t idx) const = 0;
};

class AbstractPointCloud
{
public:
    using Ptr = std::shared_ptr<AbstractPointCloud>;

    // ToDo: Add const iterator which uses the virtual methodes!

    virtual bool hasChannel(int type) const = 0;

    virtual std::vector<double>& getChannel(int type) = 0;
    virtual const std::vector<double>& getChannel(int type) const = 0;

    virtual std::size_t size() const;

    virtual const AbstractPoint& operator[](std::size_t idx) const = 0;

    // ToDo:
    //virtual void apply(std::function<void(AbstractPoint&)>) = 0;
};


    
}}

#endif /* end of include guard: SEMPR_ENTITY_SPATIAL_ABSTRACTPOINTCLOUD_HPP_ */
