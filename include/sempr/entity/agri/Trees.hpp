#ifndef SEMPR_ENTITY_TREES_HPP
#define SEMPR_ENTITY_TREES_HPP

#include <sempr/entity/agri/AgriEntity.hpp>

namespace sempr { namespace entity { namespace agri {

#pragma db object
/**
 * @brief The Trees class
 */
class Trees : public AgriEntity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Trees>;

    Trees();
    Trees(const sempr::core::IDGenBase*);
    Trees(std::string type, std::string identifier, const core::IDGenBase* idgen);

    std::string type() { return m_type; }
    std::string identifier() { return m_identifier; }

    virtual ~Trees();
private:
    friend class odb::access;

    std::string m_type;
    std::string m_identifier;

};

} } }


#endif // SEMPR_ENTITY_TREES_HPP
