#include <sempr/entity/agri/Path.hpp>
#include "Path_odb.h"

namespace sempr { namespace entity { namespace agri {

SEMPR_ENTITY_SOURCE(Path)

Path::Path() : Path(new core::IDGen<Path>())
{
}

Path::Path(const core::IDGenBase* idgen) : AgriEntity(idgen)
{
    setDiscriminator<Path>();
}

Path::Path(std::string type, std::string identifier, const core::IDGenBase* idgen) :
    AgriEntity(idgen), m_type(type), m_identifier(identifier)
{
    setDiscriminator<Path>();
}

Path::~Path()
{

}

} } }
