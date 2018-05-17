#include <sempr/entity/spatial/GeographicCS.hpp>
#include <GeographicCS_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GeographicCS)

GeographicCS::GeographicCS()
{
    // do nothing. this is just for odb to load from the database...
}

GeographicCS::GeographicCS(const std::string& name)
    : GeographicCS(name, new core::IDGen<GeographicCS>())
{
}

GeographicCS::GeographicCS(const std::string& name, const core::IDGenBase* idgen)
    : GlobalCS(idgen)
{
    this->setDiscriminator<GeographicCS>();
    this->frame_.SetWellKnownGeogCS(name.c_str());
}

SpatialReference::Ptr GeographicCS::getRoot()
{
    return this->shared_from_base<SpatialReference>();
}

Eigen::Affine3d GeographicCS::transformationToRoot() const
{
    return Eigen::Affine3d::Identity();
}

Eigen::Affine3d GeographicCS::transformationFromRoot() const
{
    return Eigen::Affine3d::Identity();
}

}}
