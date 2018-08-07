#ifndef SEMPR_ENTITY_AGRIENTITY_HPP
#define SEMPR_ENTITY_AGRIENTITY_HPP

#include <geos/geom/Polygon.h>

#include <sempr/entity/Entity.hpp>
#include <sempr/entity/spatial/Polygon.hpp>

#include <sempr/core/IDGenUtil.hpp>

namespace spatial = sempr::entity;

namespace sempr { namespace entity { namespace agri {

#pragma db object
/** 
 * @brief The AgriEntity class is just an inherit of Polygon - so I will not need to copy all the features I want to use from Polygon.
 */
class AgriEntity : public Entity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<AgriEntity>;

    AgriEntity();
    AgriEntity(const sempr::core::IDGenBase*);

    Polygon::Ptr polygon() { return m_polygon; }
    virtual ~AgriEntity();
protected:
    friend class odb::access;

    Polygon::Ptr m_polygon;
};

}}}

#endif // SEMPR_ENTITY_AGRIENTITY_HPP
