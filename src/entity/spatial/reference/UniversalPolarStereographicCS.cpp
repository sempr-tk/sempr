#include <sempr/entity/spatial/reference/UniversalPolarStereographicCS.hpp>
#include <UniversalPolarStereographicCS_odb.h>
#include <GeographicLib/Constants.hpp>

#include <memory>
#include <cmath>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(UniversalPolarStereographicCS)

UniversalPolarStereographicCS::UniversalPolarStereographicCS() :
    north_(true)
{
}

UniversalPolarStereographicCS::UniversalPolarStereographicCS(bool north) : 
    UniversalPolarStereographicCS(north, new core::IDGen<UniversalPolarStereographicCS>())
{
}

UniversalPolarStereographicCS::UniversalPolarStereographicCS(bool north, const core::IDGenBase* idgen) : 
    ProjectionCS(idgen),
    north_(north)
{
    this->setDiscriminator<UniversalPolarStereographicCS>();
}

int UniversalPolarStereographicCS::getZone() const
{
    return 0;   // 0 as UPS Zone near the polars
}

bool UniversalPolarStereographicCS::isNorth() const
{
    return north_;
}

bool UniversalPolarStereographicCS::isEqual(const SpatialReference::Ptr other) const
{
    auto otherUPS = std::dynamic_pointer_cast<UniversalPolarStereographicCS>(other);

    if(otherUPS)
    {
        return this->north_ == otherUPS->north_;
    }
    else
    {
        return false;
    }
}

FilterPtr UniversalPolarStereographicCS::forward() const
{
    return FilterPtr(new UPSForwardFilter(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f(), GeographicLib::Constants::UPS_k0(), north_));
}

FilterPtr UniversalPolarStereographicCS::reverse() const
{
    return FilterPtr(new UPSReversFilter(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f(), GeographicLib::Constants::UPS_k0(), north_));
}


}}
