#include <sempr/entity/spatial/reference/ProjectionCS.hpp>
#include <sempr/entity/spatial/reference/GeodeticCS.hpp>
#include <sempr/entity/spatial/reference/UniversalTransverseMercatorCS.hpp>
#include <sempr/entity/spatial/reference/UniversalPolarStereographicCS.hpp>
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

FilterList ProjectionCS::to(const GlobalCS::Ptr other) const
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

int ProjectionCS::getZone() const
{
    //only for ODB support - not abstract
    return -1;
}

bool ProjectionCS::isNorth() const
{
    //only for ODB support - not abstract
    return true;
}

std::size_t ProjectionCS::directionDimension(const CardinalDirection& direction) const
{
    switch (direction)
    {
        case NORTH:
        case SOUTH:
            return 1;   //y
        case EAST:
        case WEST:
            return 0;   //x
    }

    return 0;
}


ProjectionCS::Ptr ProjectionCS::importZone(const std::string& zonestr)
{
    int zone;
    bool north;
    GeographicLib::UTMUPS::DecodeZone(zonestr, zone, north);

    if (zone > 0 && zone <= 60)
    {
        // is UTM
        return ProjectionCS::Ptr(new UniversalTransverseMercatorCS(zone, north));
    }
    else if (zone == 0)
    {
        // is UPS
        return ProjectionCS::Ptr(new UniversalPolarStereographicCS(north)); //todo
    }
    else
    {
        //invalid
        throw ProjectionException("Invalid zone string!");
        return ProjectionCS::Ptr(nullptr);
    }
}

std::string ProjectionCS::exportZone(const ProjectionCS::Ptr cs)
{
    return GeographicLib::UTMUPS::EncodeZone(cs->getZone(), cs->isNorth());
}

ProjectionCS::Ptr ProjectionCS::importEPSGZone(int epsg)
{
    int zone;
    bool north;
    GeographicLib::UTMUPS::DecodeEPSG(epsg, zone, north);

    if (zone > 0 && zone <= 60)
    {
        // is UTM
        return ProjectionCS::Ptr(new UniversalTransverseMercatorCS(zone, north));
    }
    else if (zone == 0)
    {
        // is UPS
        return ProjectionCS::Ptr(new UniversalPolarStereographicCS(north));  //todo
    }
    else
    {
        //invalid
        throw ProjectionException("Invalid zone string!");
        return ProjectionCS::Ptr(nullptr);
    }
}

int ProjectionCS::exportEPSGZone(const ProjectionCS::Ptr cs)
{
    return GeographicLib::UTMUPS::EncodeEPSG(cs->getZone(), cs->isNorth());
}



}}
