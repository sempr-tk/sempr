#ifndef SEMPR_ENTITY_WATER_HPP
#define SEMPR_ENTITY_WATER_HPP

#include <sempr/entity/agri/AgriEntity.hpp>

namespace sempr { namespace entity { namespace agri {

#pragma db object
/**
 * @brief The Water class
 */
class Water : public AgriEntity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Water>;

    Water();
    Water(const sempr::core::IDGenBase*);
    Water(std::string type, std::string identifier, const core::IDGenBase* idgen);

    std::string type() { return m_type; }
    std::string identifier() { return m_identifier; }

    virtual ~Water();
private:
    friend class odb::access;

    std::string m_identifier;
    std::string m_type;

};

} } }


#endif // SEMPR_ENTITY_WATER_HPP
