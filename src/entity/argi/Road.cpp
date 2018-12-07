#include <sempr/entity/agri/Road.hpp>
#include "Road_odb.h"

namespace sempr { namespace entity { namespace agri {

SEMPR_ENTITY_SOURCE(Road)

Road::Road() : Road(new core::IDGen<Road>())
{
}

Road::Road(const core::IDGenBase* idgen) : AgriEntity(idgen)
{
    setDiscriminator<Road>();
    _type = AgriType::ROAD;
}

Road::Road(const std::string& name, const core::IDGenBase* idgen) :
    AgriEntity(name, idgen)
{
    setDiscriminator<Road>();
    _type = AgriType::ROAD;
}

Road::~Road()
{

}

} } }
