#ifndef SEMPR_ENTITY_PATH_HPP
#define SEMPR_ENTITY_PATH_HPP

#include <sempr/entity/agri/AgriEntity.hpp>

namespace sempr { namespace entity { namespace agri {

#pragma db object
/**
 * @brief The Path class
 */
class Path : public AgriEntity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Path>;

    Path();
    Path(const sempr::core::IDGenBase*);
    Path(std::string seed, std::string identifier, const core::IDGenBase* idgen);

    std::string type() { return m_type; }
    std::string identifier() { return m_identifier; }

    virtual ~Path();
private:
    friend class odb::access;

    std::string m_identifier;
    std::string m_type;

};

} } }


#endif // SEMPR_ENTITY_PATH_HPP
