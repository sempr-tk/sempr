#include <sempr/entity/spatial/filter/ProjectionFilter.hpp>
#include <cmath>

namespace sempr {

UTMFilter::UTMFilter(double a, double f, double k0, int zone) :
    tm_(GeographicLib::TransverseMercator(a, f, k0)),
    zone_(zone)
{
}

void UTMFilter::filter_ro(const geom::Coordinate* coordinate)
{
    // do nothing
    throw ProjectionException("No read-only filter for a local transformation!");
}

void UTMFilter::filter_rw(geom::Coordinate* coordinate) const
{
    assert(0); //like geos geom will do!
}

double UTMFilter::CentralMeridian(int zone)
{ 
    if (zone >= GeographicLib::UTMUPS::MINZONE && zone <= GeographicLib::UTMUPS::MAXZONE)
    {
        return double(6 * zone - 183); 
    }
    else
    {
        return GeographicLib::UTMUPS::INVALID;
    }
}

UTMForwardFilter::UTMForwardFilter(double a, double f, double k0, int zone) :
    UTMFilter(a, f, k0, zone)
{
}

void UTMForwardFilter::filter_rw(geom::Coordinate* coordinate) const
{
    //todo
}


UTMReversFilter::UTMReversFilter(double a, double f, double k0, int zone) :
    UTMFilter(a, f, k0, zone)
{
}

void UTMReversFilter::filter_rw(geom::Coordinate* coordinate) const
{
    //todo
}



UPSFilter::UPSFilter(double a, double f, double k0, bool north) :
    ps_(GeographicLib::PolarStereographic(a, f, k0)),
    north_(north)
{
}

void UPSFilter::filter_ro(const geom::Coordinate* coordinate)
{
    // do nothing
    throw ProjectionException("No read-only filter for a local transformation!");
}

void UPSFilter::filter_rw(geom::Coordinate* coordinate) const
{
    assert(0); //like geos geom will do!
}


UPSForwardFilter::UPSForwardFilter(double a, double f, double k0, bool north) :
    UPSFilter(a, f, k0, north)
{
}

void UPSForwardFilter::filter_rw(geom::Coordinate* coordinate) const
{
    //todo
}


UPSReversFilter::UPSReversFilter(double a, double f, double k0, bool north) :
    UPSFilter(a, f, k0, north)
{
}

void UPSReversFilter::filter_rw(geom::Coordinate* coordinate) const
{
    //todo
}



} /* sempr */
