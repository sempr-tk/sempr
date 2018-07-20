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
    LocalTangentPlaneCS(origin, new core::IDGen<LocalTangentPlaneCS>())
{
}

LocalTangentPlaneCS::LocalTangentPlaneCS(const geom::Coordinate& origin, const core::IDGenBase* idgen) : 
    GeocentricCS(idgen),
    lat0_(origin.x),
    lon0_(origin.y),
    h0_(origin.z)
{
    this->setDiscriminator<LocalTangentPlaneCS>();


    if (std::isnan(origin.z))
        throw GeocentricException("No valid z component of the LTP origin (dont use the Coordinate default constructor!)");

}

bool LocalTangentPlaneCS::isEqual(const GlobalCS::Ptr other)
{
    auto otherLTG = std::dynamic_pointer_cast<LocalTangentPlaneCS>(other);

    if(otherLTG)
    {
        return geom::Coordinate(lat0_, lon0_, h0_).equals3D(geom::Coordinate(otherLTG->lat0_, otherLTG->lon0_, otherLTG->h0_));
    }
    else
    {
        return false;
    }
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
