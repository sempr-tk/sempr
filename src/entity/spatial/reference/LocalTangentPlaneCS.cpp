#include <sempr/entity/spatial/reference/LocalTangentPlaneCS.hpp>
#include <LocalTangentPlaneCS_odb.h>
#include <GeographicLib/Constants.hpp>

#include <memory>
#include <cmath>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(LocalTangentPlaneCS)

LocalTangentPlaneCS::LocalTangentPlaneCS(const geom::Coordinate& origin) : 
    GeocentricCS(origin, new core::IDGen<LocalTangentPlaneCS>())
{
    if (std::isnan(origin.z))
        throw GeodeticException("No valid z component of the LTP origin (dont use the Coordinate default constructor!)");
}

LocalTangentPlaneCS::LocalTangentPlaneCS(const geom::Coordinate& origin, const core::IDGenBase* idgen) : 
    GeocentricCS(idgen),
    origin_(origin)
{
    this->setDiscriminator<LocalTangentPlaneCS>();
}

bool LocalTangentPlaneCS::isEqual(const GlobalCS::Ptr other)
{
    auto otherLTG = std::dynamic_pointer_cast<LocalTangentPlaneCS>(other);

    if(otherLTG)
    {
        return origin_.equals3D(otherLTG->origin_);
    }
    else
    {
        return false;
    }
}

FilterPtr LocalTangentPlaneCS::forward() const
{
    return FilterPtr(new LTGForwardFilter(origin_.x, origin_.y, origin_.z, GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
}

FilterPtr LocalTangentPlaneCS::reverse() const
{
    return FilterPtr(new LTGReverseFilter(origin_.x, origin_.y, origin_.z, GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());
}


}}
