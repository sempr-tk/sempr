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
    _type = AgriType::FIELD;

}

Field::Field(const std::string &seed, const std::string &name, const core::IDGenBase* idgen) :
    AgriEntity(name, idgen), _seed(seed)
{
    setDiscriminator<Field>();
    _type = AgriType::FIELD;

}

Field::~Field()
{

}

} } }
