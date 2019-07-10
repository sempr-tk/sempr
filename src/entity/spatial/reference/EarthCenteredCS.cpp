#include <sempr/entity/spatial/reference/EarthCenteredCS.hpp>
#include <EarthCenteredCS_odb.h>
#include <GeographicLib/Constants.hpp>

#include <memory>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(EarthCenteredCS)

EarthCenteredCS::EarthCenteredCS() : 
    GeocentricCS(new core::IDGen<EarthCenteredCS>())
{
}

EarthCenteredCS::EarthCenteredCS(const core::IDGenBase* idgen) : 
    GeocentricCS(idgen)
{
    this->setDiscriminator<EarthCenteredCS>();
}

FilterPtr EarthCenteredCS::forward() const
{
    return FilterPtr(new ECEFForwardFilter(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f()));
}

FilterPtr EarthCenteredCS::reverse() const
{
    return FilterPtr(new ECEFReverseFilter(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f()));
}

std::size_t EarthCenteredCS::directionDimension(const CardinalDirection& direction) const
{
    switch (direction)
    {
        case NORTH:
        case SOUTH:
            return 2;   //z
        case EAST:
        case WEST:
            return 1;   //y
    }

    return 0;
}


}}
