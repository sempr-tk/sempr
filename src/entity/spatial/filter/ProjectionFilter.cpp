#include <sempr/entity/spatial/filter/ProjectionFilter.hpp>
#include <cmath>

namespace sempr {

UTMFilter::UTMFilter(double a, double f, double k0, int zone, bool north) :
    tm_(GeographicLib::TransverseMercator(a, f, k0)),
    zone_(zone),
    falseeasting_(5e5),
    falsenorthing_(north ? 0 : 100e5)
{
    done_ = false;
    changed_ = false;

    if (!(zone >= GeographicLib::UTMUPS::MINZONE && zone <= GeographicLib::UTMUPS::MAXZONE))
        throw ProjectionException("Zone is not in range!");
    
}

void UTMFilter::filter_ro(const geom::CoordinateSequence& seq, std::size_t i)
{
    // do nothing
    throw ProjectionException("No read-only filter for a utm transformation!");
}

void UTMFilter::filter_rw(geom::CoordinateSequence& seq, std::size_t i)
{
    assert(0); //like geos geom will do!
}

double UTMFilter::centralMeridian(int zone)
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

bool UTMFilter::isDone () const
{
    return done_;
}

bool UTMFilter::isGeometryChanged () const
{
    return changed_;
}

UTMForwardFilter::UTMForwardFilter(double a, double f, double k0, int zone, bool north) :
    UTMFilter(a, f, k0, zone, north)
{
}

void UTMForwardFilter::filter_rw(geom::CoordinateSequence& seq, std::size_t i)
{
    //calculate central meridian of the zone:
    const double lon0 = centralMeridian(zone_);

    for(; i < seq.getSize(); i++)
    {
        geom::Coordinate utmCoord;
        
        auto wgsCoord = seq.getAt(i); // x = lat; y = lon; z = h

        tm_.Forward(lon0, wgsCoord.x, wgsCoord.y, utmCoord.x, utmCoord.y);
        utmCoord.x += falseeasting_;
        utmCoord.y += falsenorthing_;

        seq.setAt(utmCoord, i);
        changed_ = true;
    }

    done_ = true;
}


UTMReversFilter::UTMReversFilter(double a, double f, double k0, int zone, bool north) :
    UTMFilter(a, f, k0, zone, north)
{
}

void UTMReversFilter::filter_rw(geom::CoordinateSequence& seq, std::size_t i)
{
    //calculate central meridian of the zone:
    const double lon0 = centralMeridian(zone_);

    for(; i < seq.getSize(); i++)
    {
        geom::Coordinate wgsCoord;
        
        auto utmCoord = seq.getAt(i);

        utmCoord.x -= falseeasting_;
        utmCoord.y -= falsenorthing_;

        tm_.Reverse(lon0, utmCoord.x, utmCoord.y, wgsCoord.x, wgsCoord.y);

        seq.setAt(wgsCoord, i);
        changed_ = true;
    }

    done_ = true;
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
