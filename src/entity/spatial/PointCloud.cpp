#include <sempr/entity/spatial/PointCloud.hpp>
#include <PointCloud_odb.h>

#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(PointCloud)

const double& CoordinatePoint::operator[](std::size_t idx) const
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


PointCloud::PointCloud() :
    PointCloud(new sempr::core::IDGen<PointCloud>())
{
}

PointCloud::PointCloud(const sempr::core::IDGenBase* idgen) :
    sempr::entity::MultiPoint(idgen)
{
    setDiscriminator<PointCloud>();

}

PointCloud::~PointCloud()
{

}


bool PointCloud::hasChannel(int type) const
{
    auto it = channels_.find(type);

    return it != channels_.end();
}

void PointCloud::setChannel(int type, const Channel& channel)
{
    if(channel.size() != size())
        throw std::exception(); // no equial size of points and channel information

    channels_[type] = channel;
}

AbstractChannel& PointCloud::getChannel(int type)
{
    if (!hasChannel(type))
        throw std::out_of_range("Channel " + std::to_string(type) + " not existing.");

    return channels_[type];
}


const AbstractChannel& PointCloud::getChannel(int type) const
{
    if (!hasChannel(type))
        throw std::out_of_range("Channel " + std::to_string(type) + " not existing.");

    return channels_.at(type);
}

std::size_t PointCloud::size() const
{
    return getGeometry()->getNumGeometries(); // each point is a geometry so the num of geometries shall be equal to the num of points!
}


const AbstractPoint::Ptr PointCloud::operator[](std::size_t idx) const
{
    // Note: this a ineffective way to it because for each call it will create a copy of the coordinate on the heap!
    // But this version is safe!
    return std::make_shared<CoordinatePoint>(*getGeometry()->getGeometryN(idx)->getCoordinate());
}

const CoordinatePoint PointCloud::at(std::size_t idx) const
{
    return CoordinatePoint(*getGeometry()->getGeometryN(idx)->getCoordinate());
}

geom::Coordinate& PointCloud::at(std::size_t idx)
{
    return *const_cast<geom::Coordinate*>(getGeometry()->getGeometryN(idx)->getCoordinate());
}

}}
