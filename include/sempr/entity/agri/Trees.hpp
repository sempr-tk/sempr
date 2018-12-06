#ifndef SEMPR_ENTITY_TREES_HPP
#define SEMPR_ENTITY_TREES_HPP

#include <sempr/entity/agri/AgriEntity.hpp>

namespace sempr { namespace entity { namespace agri {

#pragma db object

class Trees : public AgriEntity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Trees>;

    Trees();
    Trees(const sempr::core::IDGenBase*);
    Trees(std::string name, const core::IDGenBase* idgen);

    virtual ~Trees();
private:
    friend class odb::access;
};

} } }


#endif // SEMPR_ENTITY_TREES_HPP
