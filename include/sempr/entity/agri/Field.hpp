#ifndef FIELD_HPP
#define FIELD_HPP

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
    Field(std::string seed, std::string identifier, const core::IDGenBase* idgen);

    std::string seed() { return m_seed; }
    std::string identifier() { return m_identifier; }

    virtual ~Field();
private:
    friend class odb::access;

    std::string m_seed;
    std::string m_identifier;

};

} } }


#endif // FIELD_HPP
