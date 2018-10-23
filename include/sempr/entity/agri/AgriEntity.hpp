#ifndef SEMPR_ENTITY_AGRIENTITY_HPP
#define SEMPR_ENTITY_AGRIENTITY_HPP

#include <geos/geom/Polygon.h>

#include <sempr/entity/Entity.hpp>
#include <sempr/entity/spatial/Polygon.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>

#include <sempr/core/IDGenUtil.hpp>

namespace spatial = sempr::entity;

namespace sempr { namespace entity { namespace agri {

#pragma db object
/** 
 * @brief The AgriEntity class
 */
class AgriEntity : public Entity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<AgriEntity>;

    AgriEntity();
    AgriEntity(const sempr::core::IDGenBase*);
    AgriEntity(RDFPropertyMap::Ptr properties, const sempr::core::IDGenBase*);

    Polygon::Ptr polygon() { return m_polygon; }
    RDFPropertyMap::Ptr properties() { return m_properties; }

    double low() { return m_low; }
    double high() { return m_high; }
    void low(double low) { m_low = low; }
    void high(double high) { m_high = high; }

    virtual ~AgriEntity();
protected:
    friend class odb::access;

    Polygon::Ptr m_polygon;
    RDFPropertyMap::Ptr m_properties;

    double m_low;
    double m_high;
};

}}}

#endif // SEMPR_ENTITY_AGRIENTITY_HPP
