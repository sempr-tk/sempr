#include <sempr/entity/spatial/PointCloud.hpp>
#include <PointCloud_odb.h>

#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(PointCloud)

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


const AbstractPoint& PointCloud::operator[](std::size_t idx) const
{
    static CoordinatePoint* ptr = new CoordinatePoint(*getGeometry()->getGeometryN(idx)->getCoordinate());

    // ToDo:    By the use of static:
    //          This is not a safe way to do it! This will return a reference to a singlton wrapper that will be changed by every call. 
    //          So its not thread safe and user may not store the reference itself, because it will change!
    // Note:    Replacement new with a double inhertance object will not work because the geos Coordinate habe no virtual elements ans so no vtable.
    // Note:    To use shared pointer is mostly safe but slow because you need to alloc heap space for each call.

    ptr = new (ptr) CoordinatePoint(*getGeometry()->getGeometryN(idx)->getCoordinate());    // One Element Heap Cache by placement new.

    return *ptr;
}

}}
