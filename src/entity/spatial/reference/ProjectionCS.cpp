#include <sempr/entity/spatial/reference/ProjectionCS.hpp>
#include <sempr/entity/spatial/reference/GeodeticCS.hpp>
#include <ProjectionCS_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(ProjectionCS)

ProjectionCS::ProjectionCS() 
    : GlobalCS(new core::IDGen<ProjectionCS>())
{
    
}

ProjectionCS::ProjectionCS(double k0, const std::string& zone, bool north) 
    : ProjectionCS(k0, zone, north, new core::IDGen<ProjectionCS>())
{
}

ProjectionCS::ProjectionCS(double k0, const std::string& zone, bool north, const core::IDGenBase* idgen) : 
    GlobalCS(idgen),
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


ProjectionCS::Ptr ProjectionCS::CreateUTM(int zone)
{
    ProjectionCS::Ptr cs(new ProjectionCS(0.9996, std::to_string(zone), true));

    return cs;
}

ProjectionCS::Ptr ProjectionCS::CreateUPS(bool north)
{
    ProjectionCS::Ptr cs(new ProjectionCS(0.994, std::to_string(GeographicLib::UTMUPS::UPS), north));

    return cs;
}

ProjectionCS::Ptr ProjectionCS::CreateMGRS(const std::string& zone)
{
    ProjectionCS::Ptr cs(new ProjectionCS(1, zone, true));

    return cs;
}


FilterList ProjectionCS::to(const GlobalCS::Ptr other)
{
    //transform from this (Geodetic)
    auto otherGeodetic = std::dynamic_pointer_cast<GeodeticCS>(other);

    if (typeid(*other) == typeid(*this))
    {
        // same cs - nothing to do
        return FilterList();
    }
    else if (otherGeodetic)
    {
        FilterList list;
        list.push_back(reverse());
        return list;
    }
    else
    {
        //the other CS is another projection system or a geocentric
        FilterList list;
        list.push_back(reverse());
        list.push_back(GlobalCS::forward(other));
        return list;
    }

    return FilterList();
}

FilterPtr ProjectionCS::forward() const
{
    //only for ODB support - not abstract
    throw ProjectionException("There is no forward for a general projection cs!");
    return FilterPtr(nullptr);
}

FilterPtr ProjectionCS::reverse() const
{
    //only for ODB support - not abstract
    throw ProjectionException("There is no reverse for a general projection cs!");
    return FilterPtr(nullptr);
}




}}
