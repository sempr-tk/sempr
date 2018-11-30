#ifndef SEMPR_ENTITY_FIELD_HPP
#define SEMPR_ENTITY_FIELD_HPP

#include <sempr/entity/agri/AgriEntity.hpp>

namespace sempr { namespace entity { namespace agri {

#pragma db object
/**
 * @brief The Field class
 */
class Field : public AgriEntity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Field>;

    Field();
    Field(const sempr::core::IDGenBase*);
    Field(std::string seed, std::string name, const core::IDGenBase* idgen);

    std::string seed() { return _seed; }
    void seed(std::string seed) { _seed = seed; }

    virtual ~Field();
private:
    friend class odb::access;

    std::string _seed;
};

} } }


#endif // SEMPR_ENTITY_FIELD_HPP
