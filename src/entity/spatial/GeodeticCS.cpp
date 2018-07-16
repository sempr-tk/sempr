#include <sempr/entity/spatial/GeodeticCS.hpp>
#include <GeodeticCS_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeodeticCS)

GeodeticCS::GeodeticCS()
{
    // do nothing. this is just for odb to load from the database...
}

GeodeticCS::GeodeticCS(const std::string& name)
    : GeodeticCS(name, new core::IDGen<GeodeticCS>())
{
}

GeodeticCS::GeodeticCS(const std::string& name, const core::IDGenBase* idgen)
    : GlobalCS(idgen)
{
    this->setDiscriminator<GeodeticCS>();
    //this->frame_.SetWellKnownGeogCS(name.c_str());
}


}}
