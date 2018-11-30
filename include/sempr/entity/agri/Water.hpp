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
    Water(std::string name, const core::IDGenBase* idgen);

    virtual ~Water();
private:
    friend class odb::access;
};

} } }


#endif // SEMPR_ENTITY_WATER_HPP
