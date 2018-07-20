#include <sempr/entity/spatial/reference/GeodeticCS.hpp>
#include <GeodeticCS_odb.h>

#include <sempr/entity/spatial/filter/GeodeticFilter.hpp>

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

    if (isEqual(other))
    {
        // same cs - nothing to do
        return FilterList();
    }
    else
    {
        //this is geodetic so its easy to get forward to other
        FilterList list;
        FilterPtr forwardToOther = GlobalCS::forward(other);
        list.push_back(forwardToOther);

        return list;
    }

}

FilterPtr GeodeticCS::forward() const
{
    //nothing to do - still in geodectic
    throw GeodeticException("There is no forward for a geodetic cs to a geodetic cs!");
    return FilterPtr(nullptr);
}

FilterPtr GeodeticCS::reverse() const
{
    //nothing to do - still in geodectic
    throw GeodeticException("There is no reverse for a geodetic cs to a geodetic cs!");
    return FilterPtr(nullptr);
}




}}
