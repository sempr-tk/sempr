#include <sempr/entity/spatial/ProjectionCS.hpp>
#include <ProjectionCS_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(ProjectionCS)

ProjectionCS::ProjectionCS() 
    : GlobalCS(new core::IDGen<ProjectionCS>())
{
    
}

ProjectionCS::ProjectionCS(double k0, const std::string& zone, bool north, GeodeticReference reference) 
    : ProjectionCS(k0, zone, north, reference, new core::IDGen<ProjectionCS>())
{
}

ProjectionCS::ProjectionCS(double k0, const std::string& zone, bool north, GeodeticReference reference, const core::IDGenBase* idgen) : 
    GlobalCS(idgen, reference),
    k0_(k0),
    zone_(zone),
    north_(north)
{
    this->setDiscriminator<ProjectionCS>();
}

int ProjectionCS::getUTMZone() const
{
    return std::stoi(zone_);
}

void ProjectionCS::setUTMZone(int zone)
{
    zone_ = std::to_string(zone);
}

void ProjectionCS::setRegion(double lat, double lon)
{
    auto zone = GeographicLib::UTMUPS::StandardZone(lat, lon);
    this->setUTMZone(zone);

    north_ = lat >= 0;
}

bool ProjectionCS::getNorth() const
{
    return north_;
}

void ProjectionCS::setNorth(bool north)
{
    north_ = north;
}


ProjectionCS::Ptr ProjectionCS::CreateUTM(int zone, GeodeticReference reference)
{
    ProjectionCS::Ptr cs(new ProjectionCS(0.9996, std::to_string(zone), true, reference));

    return cs;
}

ProjectionCS::Ptr ProjectionCS::CreateUPS(bool north, GeodeticReference reference)
{
    ProjectionCS::Ptr cs(new ProjectionCS(0.994, std::to_string(GeographicLib::UTMUPS::UPS), north, reference));

    return cs;
}

ProjectionCS::Ptr ProjectionCS::CreateMGRS(const std::string& zone)
{
    ProjectionCS::Ptr cs(new ProjectionCS(1, zone, true, GeodeticReference::WGS84));

    return cs;
}

}}
