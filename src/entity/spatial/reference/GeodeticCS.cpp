#include <sempr/entity/spatial/reference/GeodeticCS.hpp>
#include <GeodeticCS_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeodeticCS)

GeodeticCS::GeodeticCS()
    : GeodeticCS(new core::IDGen<GeodeticCS>())
{
}

GeodeticCS::GeodeticCS(const core::IDGenBase* idgen)
    : GlobalCS(idgen)
{
    this->setDiscriminator<GeodeticCS>();
}

FilterList GeodeticCS::to(const GlobalCS::Ptr other)
{
    //this is geodetic so its easy to get forward to other
    FilterList list;
    std::shared_ptr<geos::geom::CoordinateFilter> forwardToOther = froward(other); //todo
    list.push_back(forwardToOther);

    return list;
}
/*
std::shared_ptr<geos::geom::CoordinateFilter> GeodeticCS::froward() const
{
    //nothing to do - still in geodectic

    //ToDo: return a empty filter

    //only for ODB support
    throw GeodeticException("There is no forward transformation from geodetic to geodetic!");
}

std::shared_ptr<geos::geom::CoordinateFilter> GeodeticCS::reverse() const
{
    //nothing to do - still in geodectic

    //ToDo: return a empty filter

    //only for ODB support
    throw GeodeticException("There is no reverse transformation from geodetic to geodetic!");
}
*/



}}
