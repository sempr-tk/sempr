#ifndef SEMPR_ENTITY_ROAD_HPP
#define SEMPR_ENTITY_ROAD_HPP

#include <sempr/entity/agri/AgriEntity.hpp>

namespace sempr { namespace entity { namespace agri {

#pragma db object
/**
 * @brief The Road class
 */
class Road : public AgriEntity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Road>;

    Road();
    Road(const sempr::core::IDGenBase*);
    Road(std::string name, const core::IDGenBase* idgen);

    virtual ~Road();
private:
    friend class odb::access;
};

} } }


#endif // SEMPR_ENTITY_ROAD_HPP
