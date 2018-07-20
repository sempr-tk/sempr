#include <sempr/entity/spatial/filter/GeocentricFilter.hpp>
#include <cmath>

namespace sempr {

/// ECEF Filter

ECEFFilter::ECEFFilter(double a, double f) : 
    ecef_(GeographicLib::Geocentric(a, f))
{
    done_ = false;
    changed_ = false;
}

bool ECEFFilter::isDone () const
{
    return done_;
}

bool ECEFFilter::isGeometryChanged () const
{
    return changed_;
}

void ECEFFilter::filter_rw(geos::geom::CoordinateSequence& seq, std::size_t i)
{
    assert(0); //like geos geom will do!
}

void ECEFFilter::filter_ro(const geos::geom::CoordinateSequence& seq, std::size_t i)
{
    throw GeocentricException("No read-only filter for a ecef transformation!");
}


ECEFForwardFilter::ECEFForwardFilter(double a, double f) :
    ECEFFilter(a, f)
{
}

void ECEFForwardFilter::filter_rw(geos::geom::CoordinateSequence& seq, std::size_t i)
{
    for(; i < seq.getSize(); i++)
    {
        geos::geom::Coordinate ecefCoord;
        
        auto wgsCoord = seq.getAt(i); // x = lat; y = lon; z = h
        ecef_.Forward(wgsCoord.x, wgsCoord.y, wgsCoord.z, ecefCoord.x, ecefCoord.y, ecefCoord.z);

        seq.setAt(ecefCoord, i);
        changed_ = true;
    }

    done_ = true;
}

ECEFReverseFilter::ECEFReverseFilter(double a, double f) :
    ECEFFilter(a, f)
{
}

void ECEFReverseFilter::filter_rw(geos::geom::CoordinateSequence& seq, std::size_t i)
{
    for(; i < seq.getSize(); i++)
    {
        geos::geom::Coordinate wgsCoord;
        
        auto ecefCoord = seq.getAt(i); // x = lat; y = lon; z = h
        ecef_.Reverse(ecefCoord.x, ecefCoord.y, ecefCoord.z, wgsCoord.x, wgsCoord.y, wgsCoord.z);

        seq.setAt(wgsCoord, i);
        changed_ = true;
    }
    
    done_ = true;
}


/// LTP Filter


LTGFilter::LTGFilter(double lat0, double lon0, double h0, double a, double f) : 
    ltg_( GeographicLib::LocalCartesian(lat0, lon0, h0, GeographicLib::Geocentric(a, f)) ) 
{
    done_ = false;
    changed_ = false;
}

bool LTGFilter::isDone () const
{
    return done_;
}

bool LTGFilter::isGeometryChanged () const
{
    return changed_;
}

void LTGFilter::filter_rw(geos::geom::CoordinateSequence& seq, std::size_t i)
{
    assert(0); //like geos geom will do!
}

void LTGFilter::filter_ro(const geos::geom::CoordinateSequence& seq, std::size_t i)
{
    throw GeocentricException("No read-only filter for a ltg transformation!");
}

LTGForwardFilter::LTGForwardFilter(double lat0, double lon0, double h0, double a, double f) :
    LTGFilter(lat0, lon0, h0, a, f)
{
}

void LTGForwardFilter::filter_rw(geos::geom::CoordinateSequence& seq, std::size_t i)
{
    for(; i < seq.getSize(); i++)
    {
        geos::geom::Coordinate ltgCoord;
        
        auto wgsCoord = seq.getAt(i);   // x = lat; y = lon; z = h
        ltg_.Forward(wgsCoord.x, wgsCoord.y, wgsCoord.z, ltgCoord.x, ltgCoord.y, ltgCoord.z);

        seq.setAt(ltgCoord, i);
        changed_ = true;
    }

    done_ = true;
}

LTGReverseFilter::LTGReverseFilter(double lat0, double lon0, double h0, double a, double f) :
    LTGFilter(lat0, lon0, h0, a, f)
{
}

void LTGReverseFilter::filter_rw(geos::geom::CoordinateSequence& seq, std::size_t i)
{
    for(; i < seq.getSize(); i++)
    {
        geos::geom::Coordinate wgsCoord;
        
        auto ltgCoord = seq.getAt(i); // x = lat; y = lon; z = h
        ltg_.Reverse(ltgCoord.x, ltgCoord.y, ltgCoord.z, wgsCoord.x, wgsCoord.y, wgsCoord.z);

        seq.setAt(wgsCoord, i);
        changed_ = true;
    }
    
    done_ = true;
}



} /* sempr */
