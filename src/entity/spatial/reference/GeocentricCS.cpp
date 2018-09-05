#include <sempr/entity/spatial/reference/GeocentricCS.hpp>
#include <sempr/entity/spatial/reference/GeodeticCS.hpp>
#include <GeocentricCS_odb.h>

#include <memory>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeocentricCS)

GeocentricCS::GeocentricCS() : 
    GeocentricCS(new core::IDGen<GeocentricCS>())
{
}

GeocentricCS::GeocentricCS(const core::IDGenBase* idgen) : 
    GlobalCS(idgen)
{
    this->setDiscriminator<GeocentricCS>();
}

FilterList GeocentricCS::to(const GlobalCS::Ptr other) const
{
    //transform from this (Geodetic)
    auto otherGeodetic = std::dynamic_pointer_cast<GeodeticCS>(other);

    if (isEqual(other))
    {
        // same cs - nothing to do
        return FilterList();
    }
    else if (otherGeodetic)
    {
        FilterList list;
        list.push_back(reverse());
        return list;
    }
    else
    {
        //the other CS is another geocentric system or a projection
        FilterList list;
        list.push_back(reverse());
        list.push_back(GlobalCS::forward(other));
        return list;
    }

    return FilterList();
}

FilterPtr GeocentricCS::forward() const
{
    //only for ODB support - not abstract
    throw GeocentricException("There is no forward for a general geocentric cs!");
    return FilterPtr(nullptr);
}

FilterPtr GeocentricCS::reverse() const
{
    //only for ODB support - not abstract
    throw GeocentricException("There is no reverse for a general geocentric cs!");
    return FilterPtr(nullptr);
}


}}
