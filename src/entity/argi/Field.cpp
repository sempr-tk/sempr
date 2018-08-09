#include <sempr/entity/agri/Field.hpp>
#include "Field_odb.h"

namespace sempr { namespace entity { namespace agri {

SEMPR_ENTITY_SOURCE(Field)

Field::Field() : Field(new core::IDGen<Field>())
{
}

Field::Field(const core::IDGenBase* idgen) : AgriEntity(idgen)
{
    setDiscriminator<Field>();
}

Field::Field(std::string seed, std::string identifier, const core::IDGenBase* idgen) :
    AgriEntity(idgen), m_seed(seed), m_identifier(identifier)
{
    setDiscriminator<Field>();
}

Field::~Field()
{

}

} } }
