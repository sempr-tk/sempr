#ifndef SEMPR_ENTITY_AGRIENTITY_HPP
#define SEMPR_ENTITY_AGRIENTITY_HPP

#include <geos/geom/Polygon.h>

#include <sempr/entity/Entity.hpp>
#include <sempr/entity/spatial/Polygon.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>

#include <map>

#include <sempr/core/IDGenUtil.hpp>

namespace sempr { namespace entity { namespace agri {

enum AgriType{
    AGRI = 0,
    FIELD = 1,
    ROAD = 5,
    WATER = 6,
    TREES = 7
};

static std::map<AgriType, std::string> TypeString =
{
    {AGRI, "AgriEntity"},
    {FIELD, "Field"},
    {ROAD, "Road"},
    {WATER, "Water"},
    {TREES, "Trees"}
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
    AgriEntity(const std::string& name, const sempr::core::IDGenBase*);

    Polygon::Ptr geometry() { return _geometry; }
    RDFPropertyMap::Ptr properties() { return _map; }

    double minHeight() { return _min_height; }
    double maxHeight() { return _max_height; }
    void minHeight(double min_height) { _min_height = min_height; }
    void maxHeight(double max_height) { _max_height = max_height; }

    std::string typeName() { return TypeString.find(_type)->second; }
    AgriType type() { return _type; }
    // TODO Typ-String
    std::string name() { return _name; }
    void name(const std::string& name) { _name = name; }

    virtual ~AgriEntity();
protected:
    friend class odb::access;

    Polygon::Ptr _geometry;
    RDFPropertyMap::Ptr _map;
    std::string _name;

    double _min_height;
    double _max_height;

    AgriType _type = AgriType::AGRI;
};

}}}

#endif // SEMPR_ENTITY_AGRIENTITY_HPP
