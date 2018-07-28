#include <sempr/entity/spatial/filter/ProjectionFilter.hpp>
#include <cmath>

#include <sempr/entity/spatial/reference/MilitaryGridReferenceSystem.hpp> 

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
    done_ = false;
    changed_ = false;
}

void UPSFilter::filter_ro(const geom::CoordinateSequence& seq, std::size_t i)
{
    // do nothing
    throw ProjectionException("No read-only filter for a ups transformation!");
}

void UPSFilter::filter_rw(geom::CoordinateSequence& seq, std::size_t i)
{
    assert(0); //like geos geom will do!
}

bool UPSFilter::isDone () const
{
    return done_;
}

bool UPSFilter::isGeometryChanged () const
{
    return changed_;
}


UPSForwardFilter::UPSForwardFilter(double a, double f, double k0, bool north) :
    UPSFilter(a, f, k0, north)
{
}

void UPSForwardFilter::filter_rw(geom::CoordinateSequence& seq, std::size_t i)
{
    for(; i < seq.getSize(); i++)
    {
        geom::Coordinate upsCoord;
        
        auto wgsCoord = seq.getAt(i); // x = lat; y = lon; z = h

        ps_.Forward(north_, wgsCoord.x, wgsCoord.y, upsCoord.x, upsCoord.y);

        seq.setAt(upsCoord, i);
        changed_ = true;
    }

    done_ = true;
}


UPSReversFilter::UPSReversFilter(double a, double f, double k0, bool north) :
    UPSFilter(a, f, k0, north)
{
}

void UPSReversFilter::filter_rw(geom::CoordinateSequence& seq, std::size_t i)
{
    for(; i < seq.getSize(); i++)
    {
        geom::Coordinate wgsCoord;
        
        auto upsCoord = seq.getAt(i);

        ps_.Reverse(north_, upsCoord.x, upsCoord.y, wgsCoord.x, wgsCoord.y);

        seq.setAt(wgsCoord, i);
        changed_ = true;
    }

    done_ = true;
}


MGRSFilter::MGRSFilter(const std::string& GZDSquareID) :
    GZDSquareID_(GZDSquareID)
{
    done_ = false;
    changed_ = false;
}

void MGRSFilter::filter_ro(const geom::CoordinateSequence& seq, std::size_t i)
{
    // do nothing
    throw ProjectionException("No read-only filter for a mgrs transformation!");
}

void MGRSFilter::filter_rw(geom::CoordinateSequence& seq, std::size_t i)
{
    assert(0); //like geos geom will do!
}

bool MGRSFilter::isDone () const
{
    return done_;
}

bool MGRSFilter::isGeometryChanged () const
{
    return changed_;
}

MGRSForwardFilter::MGRSForwardFilter(const std::string& GZDSquareID) :
    MGRSFilter(GZDSquareID)
{
}

void MGRSForwardFilter::filter_rw(geom::CoordinateSequence& seq, std::size_t idx)
{
    const std::size_t startIdx = idx;

    std::vector<geom::Coordinate> tmp(seq.getSize());

    for(std::size_t i = startIdx; i < seq.getSize(); i++)
    {
        auto wgsCoord = seq.getAt(i); // x = lat; y = lon; z = h

        int zone;
        bool northp;
        double x, y;
        GeographicLib::UTMUPS::Forward(wgsCoord.x, wgsCoord.y, zone, northp, x, y);

        std::string mgrs;
        GeographicLib::MGRS::Forward(zone, northp, x, y, wgsCoord.x, 8, mgrs);  //precision of 8 digits

        std::string squareID;
        geom::Coordinate mgrsCoord;

        entity::MilitaryGridReferenceSystem::splitMGRS(mgrs, squareID, mgrsCoord.x, mgrsCoord.y);

        if (squareID != GZDSquareID_)
            throw ProjectionZoneMissmatch("Given MGRS zone does not match the calculated one!");

        //store in tmp
        tmp[i] = mgrsCoord;
    }

    // no zone missmatch so the transformation could be applied:
    for(std::size_t i = startIdx; i < seq.getSize(); i++)
    {
        seq.setAt(tmp[i], i);
        changed_ = true;
    }

    done_ = true;
}


MGRSReversFilter::MGRSReversFilter(const std::string& GZDSquareID) :
    MGRSFilter(GZDSquareID)
{
}

void MGRSReversFilter::filter_rw(geom::CoordinateSequence& seq, std::size_t idx)
{
    const std::size_t startIdx = idx;

    std::vector<geom::Coordinate> tmp(seq.getSize());

    for(std::size_t i = startIdx; i < seq.getSize(); i++)
    {
        auto mgrsCoord = seq.getAt(i); // x = lat; y = lon; z = h

        std::string mgrsStr = entity::MilitaryGridReferenceSystem::buildMGRS(GZDSquareID_, mgrsCoord.x, mgrsCoord.y, "");

        int zone, prec;
        bool northp;
        double x, y;
        GeographicLib::MGRS::Reverse(mgrsStr, zone, northp, x, y, prec);

        geom::Coordinate wgsCoord;
        GeographicLib::UTMUPS::Reverse(zone, northp, x, y, wgsCoord.x, wgsCoord.y);

        seq.setAt(wgsCoord, i);
        changed_ = true;
    }

    done_ = true;
}



} /* sempr */
