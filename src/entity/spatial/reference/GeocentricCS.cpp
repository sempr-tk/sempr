#include <sempr/entity/spatial/reference/GeocentricCS.hpp>
#include <GeocentricCS_odb.h>

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

FilterList GeocentricCS::to(const GlobalCS::Ptr other)
{
    //only for ODB support - not abstract
    return FilterList();
}
/*
std::shared_ptr<geos::geom::CoordinateFilter> GeocentricCS::froward() const
{
    //only for ODB support - not abstract
    throw GeocentricException();
}

std::shared_ptr<geos::geom::CoordinateFilter> GeocentricCS::reverse() const
{
    //only for ODB support - not abstract
    throw GeocentricException();
}
*/
}}
