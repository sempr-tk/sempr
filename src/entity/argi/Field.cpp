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
    _type = FIELD;

}

Field::Field(std::string seed, std::string name, const core::IDGenBase* idgen) :
    _seed(seed), AgriEntity(name, idgen)
{
    setDiscriminator<Field>();
    _type = FIELD;

}

Field::~Field()
{

}

} } }
