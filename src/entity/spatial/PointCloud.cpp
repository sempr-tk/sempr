#include <sempr/entity/spatial/PointCloud.hpp>
#include <PointCloud_odb.h>

#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(PointCloud)



CoordinatePoint::CoordinatePoint(const geom::Coordinate& coord) :
    coord_(coord)
{

}
/*
double& CoordinatePoint::operator[](std::size_t idx)
{
    if (idx == 0)
        return coord_.x;
    else if (idx == 1)
        return coord_.y;
    else if (idx == 2)
        return coord_.z;
    else
        throw std::out_of_range(); // Out of boundary!

    return coord_.z;
}
*/
const double& CoordinatePoint::operator[](std::size_t idx) const
{
    if (idx == 0)
        return coord_.x;
    else if (idx == 1)
        return coord_.y;
    else if (idx == 2)
        return coord_.z;
    else
        throw std::out_of_range(""); // Out of boundary!

    return coord_.z;
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

void PointCloud::setChannel(int type, const std::vector<double>& channel)
{
    if(channel.size() != size())
        throw std::exception(); // no equial size of points and channel information

    channels_[type] = Channel(channel);
}

std::vector<double>& PointCloud::getChannel(int type)
{
    if (!hasChannel(type))
        throw std::out_of_range("");

    return channels_[type].channel_;
}
/*
const std::vector<double>& PointCloud::getChannel(int type) const
{
    if (hasChannel(type))
        return channels_[type].channel_;

    else
        throw std::out_of_range("");

    return std::vector<double>();
}*/

std::size_t PointCloud::size() const
{
    return getGeometry()->getNumGeometries(); // each point is a geometry so the num of geometries shall be equal to the num of points!
}


const AbstractPoint& PointCloud::operator[](std::size_t idx) const
{
    return CoordinatePoint(*getGeometry()->getGeometryN(idx)->getCoordinate());  //implicit type cast
}

}}
