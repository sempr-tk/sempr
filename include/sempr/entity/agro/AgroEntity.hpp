#ifndef SEMPR_ENTITY_AGROENTITY_HPP
#define SEMPR_ENTITY_AGROENTITY_HPP

#include <sempr/entity/Entity.hpp>
#include <sempr/entity/spatial/Polygon.hpp>
#include <geos/geom/Polygon.h>

#include <iostream>
//#include <type_traits>
//namespace geom = geos::geom;
namespace spatial = sempr::entity;

namespace sempr { namespace entity { namespace agro {

#pragma db object
/**
 * @brief The AgroEntity class is just an inherit of Polygon - so I will not need to copy all the features I want to use from Polygon.
 */
class AgroEntity : public Entity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<AgroEntity>;

    AgroEntity();
    AgroEntity(const sempr::core::IDGenBase*);

    const geos::geom::Polygon* geometry() { return m_polygon->getGeometry(); }
    virtual ~AgroEntity();
protected:
    friend class odb::access;
    Polygon::Ptr m_polygon;
};

}}}

#endif // SEMPR_ENTITY_AGROENTITY_HPP
