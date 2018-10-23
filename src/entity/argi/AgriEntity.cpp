#include <AgriEntity_odb.h>

namespace sempr { namespace entity{ namespace agri {

SEMPR_ENTITY_SOURCE(AgriEntity)

AgriEntity::AgriEntity() : AgriEntity(new core::IDGen<AgriEntity>())
{
}

AgriEntity::AgriEntity(const core::IDGenBase* idgen) : Entity(idgen)
{
    setDiscriminator<AgriEntity>();

    m_polygon = Polygon::Ptr(new Polygon());
    std::cout << id() << " created an Polygon, namely " << m_polygon->id() << '\n';

    std::string str = this->discriminator() + "/";
    //str.push_back("/");

    m_properties = RDFPropertyMap::Ptr(new RDFPropertyMap("SemanticData", str));
    std::cout << id() << " created an PropertyMap, namely " << m_properties->id() << '\n';

    registerChildEntity(m_polygon);

    registerChildEntity(m_properties);
}

AgriEntity::AgriEntity(RDFPropertyMap::Ptr properties, const core::IDGenBase* idgen) : Entity(idgen)
{
    setDiscriminator<AgriEntity>();

    m_polygon = Polygon::Ptr(new Polygon());
    std::cout << id() << " created an Polygon, namely " << m_polygon->id() << '\n';

    std::string str = this->discriminator() + "/";
    //str.push_back("/");

    m_properties = properties;

    registerChildEntity(m_polygon);

    registerChildEntity(m_properties);
}

AgriEntity::~AgriEntity()
{
}


}}}
