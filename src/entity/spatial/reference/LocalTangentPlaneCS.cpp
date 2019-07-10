#include <sempr/entity/spatial/reference/LocalTangentPlaneCS.hpp>
#include <LocalTangentPlaneCS_odb.h>
#include <GeographicLib/Constants.hpp>

#include <memory>
#include <cmath>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(LocalTangentPlaneCS)

LocalTangentPlaneCS::LocalTangentPlaneCS() :
    lat0_(0),
    lon0_(0),
    h0_(0)
{
}

LocalTangentPlaneCS::LocalTangentPlaneCS(const geom::Coordinate& origin) : 
    LocalTangentPlaneCS(origin.x, origin.y, origin.z, new core::IDGen<LocalTangentPlaneCS>())
{
}

LocalTangentPlaneCS::LocalTangentPlaneCS(double lat0, double lon0, double h0) :
    LocalTangentPlaneCS(lat0, lon0, h0, new core::IDGen<LocalTangentPlaneCS>())
{
}

LocalTangentPlaneCS::LocalTangentPlaneCS(double lat0, double lon0, double h0, const core::IDGenBase* idgen) : 
    GeocentricCS(idgen),
    lat0_(lat0),
    lon0_(lon0),
    h0_(h0)
{
    this->setDiscriminator<LocalTangentPlaneCS>();


    if (std::isnan(h0))
        throw GeocentricException("No valid z component of the LTP origin (dont use the Coordinate default constructor!)");

}

bool LocalTangentPlaneCS::isEqual(const SpatialReference::Ptr other) const
{
    auto otherLTG = std::dynamic_pointer_cast<LocalTangentPlaneCS>(other);

    if(otherLTG)
    {   // check if origin is equal:
        return geom::Coordinate(lat0_, lon0_, h0_).equals3D(geom::Coordinate(otherLTG->lat0_, otherLTG->lon0_, otherLTG->h0_));
    }
    else
    {
        return false;
    }
}

std::size_t LocalTangentPlaneCS::directionDimension(const CardinalDirection& direction) const
{
    // ENU / LTG (East-North-Up)
    switch (direction)
    {
        case NORTH:
        case SOUTH:
            return 1;   //y
        case EAST:
        case WEST:
            return 0;   //x
    }

    return 0;
}

FilterPtr LocalTangentPlaneCS::forward() const
{
    return FilterPtr(new LTGForwardFilter(lat0_, lon0_, h0_, GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f()));
}

FilterPtr LocalTangentPlaneCS::reverse() const
{
    return FilterPtr(new LTGReverseFilter(lat0_, lon0_, h0_, GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f()));
}


}}
