#ifndef SEMPR_ENTITY_AGRIENTITY_HPP
#define SEMPR_ENTITY_AGRIENTITY_HPP

#include <geos/geom/Polygon.h>

#include <sempr/entity/Entity.hpp>
#include <sempr/entity/spatial/Polygon.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>

#include <sempr/core/IDGenUtil.hpp>

namespace spatial = sempr::entity;

namespace sempr { namespace entity { namespace agri {

enum AgriType{
    AGRI = 0,
    FIELD = 1,
    ROAD = 5,
    WATER = 6,
    TREES = 7
};

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
    AgriEntity(std::string name, const sempr::core::IDGenBase*);

    Polygon::Ptr geometry() { return _geometry; }
    RDFPropertyMap::Ptr properties() { return _map; }

    double low() { return _low; }
    double high() { return _high; }
    void low(double low) { _low = low; }
    void high(double high) { _high = high; }

    AgriType type() { return _type; }
    std::string name() { return _name; }
    void name(std::string name) { _name = name; }

    virtual ~AgriEntity();
protected:
    friend class odb::access;

    Polygon::Ptr _geometry;
    RDFPropertyMap::Ptr _map;
    std::string _name;

    double _low;
    double _high;

    AgriType _type = AGRI;
};

}}}

#endif // SEMPR_ENTITY_AGRIENTITY_HPP
