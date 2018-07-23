#include <sempr/entity/spatial/reference/ProjectionCS.hpp>
#include <sempr/entity/spatial/reference/GeodeticCS.hpp>
#include <sempr/entity/spatial/reference/UniversalTransverseMercatorCS.hpp>
#include <ProjectionCS_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(ProjectionCS)


ProjectionCS::ProjectionCS() : 
    ProjectionCS(new core::IDGen<ProjectionCS>())
{
}

ProjectionCS::ProjectionCS(const core::IDGenBase* idgen) : 
    GlobalCS(idgen)
{
    this->setDiscriminator<ProjectionCS>();
}

FilterList ProjectionCS::to(const GlobalCS::Ptr other)
{
    //transform from this (Geodetic)
    auto otherGeodetic = std::dynamic_pointer_cast<GeodeticCS>(other);

    if (isEqual(other))
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

ProjectionCS::Ptr ProjectionCS::importZone(const std::string& zonestr)
{
    //todo
    int zone;
    bool north;
    GeographicLib::UTMUPS::DecodeZone(zonestr, zone, north);

    if (zone > 0 && zone <= 60)
    {
        // is UTM
        return ProjectionCS::Ptr(new UniversalTransverseMercatorCS(zone, north));;
    }
    else if (zone == 0)
    {
        // is UPS
    }
    else
    {
        //invalid
        throw ProjectionException("Invalid zone string!");
        return ProjectionCS::Ptr(nullptr);
    }

    return ProjectionCS::Ptr(nullptr);
}

std::string ProjectionCS::exportZone(const ProjectionCS::Ptr cs)
{
    //todo
    return "";
}

ProjectionCS::Ptr ProjectionCS::importEPSGZone(int epsg)
{
    //todo
    return ProjectionCS::Ptr(nullptr);
}

int ProjectionCS::exportEPSGZone(const ProjectionCS::Ptr cs)
{
    //todo
    return 0;
}



}}
