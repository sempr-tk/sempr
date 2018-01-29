#include <sempr/entity/spatial/ProjectionCS.hpp>
#include <ProjectionCS_odb.h>

namespace sempr { namespace entity {

ProjectionCS::ProjectionCS() : GlobalCS(new core::IDGen<ProjectionCS>())
{
    this->setDiscriminator<ProjectionCS>();
}


// TODO error checking!
ProjectionCS::Ptr ProjectionCS::CreateUTM(int zone, bool north, const std::string& base)
{
    ProjectionCS::Ptr cs(new ProjectionCS());
    cs->frame_.SetWellKnownGeogCS(base.c_str());
    cs->frame_.SetUTM(zone, north);
    return cs;
}

ProjectionCS::Ptr ProjectionCS::CreateEquirect(double lat, double lon, const std::string &base)
{
    ProjectionCS::Ptr cs(new ProjectionCS());
    cs->frame_.SetWellKnownGeogCS(base.c_str());
    cs->frame_.SetEquirectangular(lat, lon, 0, 0);
    return cs;
}


SpatialReference::Ptr ProjectionCS::getRoot()
{
    return this->shared_from_base<SpatialReference>();
}

Eigen::Affine3d ProjectionCS::transformationToRoot() const
{
    return Eigen::Affine3d::Identity();
}

Eigen::Affine3d ProjectionCS::transformationFromRoot() const
{
    return Eigen::Affine3d::Identity();
}

}}
