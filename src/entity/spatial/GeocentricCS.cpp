#include <sempr/entity/spatial/GeocentricCS.hpp>
#include <GeocentricCS_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeocentricCS)

GeocentricCS::GeocentricCS()
{
    // do nothing. this is just for odb to load from the database...
}

GeocentricCS::GeocentricCS(const std::string& name)
    : GeocentricCS(name, new core::IDGen<GeocentricCS>())
{
}

GeocentricCS::GeocentricCS(const std::string& name, const core::IDGenBase* idgen)
    : GlobalCS(idgen)
{
    this->setDiscriminator<GeocentricCS>();
    //this->frame_.SetWellKnownGeogCS(name.c_str());
}

}}
