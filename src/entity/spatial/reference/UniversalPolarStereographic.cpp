#include <sempr/entity/spatial/reference/UniversalTransverseMercatorCS.hpp>
#include <UniversalTransverseMercatorCS_odb.h>
#include <GeographicLib/Constants.hpp>

#include <memory>
#include <cmath>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(UniversalTransverseMercatorCS)

UniversalTransverseMercatorCS::UniversalTransverseMercatorCS() :
    zone_(-1),
    north_(true)
{
}

UniversalTransverseMercatorCS::UniversalTransverseMercatorCS(int zone, bool north) : 
    UniversalTransverseMercatorCS(zone, north, new core::IDGen<UniversalTransverseMercatorCS>())
{
}

UniversalTransverseMercatorCS::UniversalTransverseMercatorCS(int zone, bool north, const core::IDGenBase* idgen) : 
    ProjectionCS(idgen),
    zone_(zone),
    north_(north)
{
    this->setDiscriminator<UniversalTransverseMercatorCS>();


    if (!(zone > 0 && zone <= 60))
        throw ProjectionException("UTM Zone is not in range!");

}

int UniversalTransverseMercatorCS::getZone() const
{
    return zone_;
}

bool UniversalTransverseMercatorCS::isNorth() const
{
    return north_;
}

bool UniversalTransverseMercatorCS::isEqual(const GlobalCS::Ptr other)
{
    auto otherUTM = std::dynamic_pointer_cast<UniversalTransverseMercatorCS>(other);

    if(otherUTM)
    {
        return this->zone_ == otherUTM->zone_ && this->north_ == otherUTM->north_;
    }
    else
    {
        return false;
    }
}

FilterPtr UniversalTransverseMercatorCS::forward() const
{
    return FilterPtr(new UTMForwardFilter(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f(), GeographicLib::Constants::UTM_k0(), zone_, north_));
}

FilterPtr UniversalTransverseMercatorCS::reverse() const
{
    return FilterPtr(new UTMReversFilter(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f(), GeographicLib::Constants::UTM_k0(), zone_, north_));
}


}}
