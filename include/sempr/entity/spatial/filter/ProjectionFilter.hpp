#ifndef SEMPR_ENTITY_SPATIAL_PROJECTIONFILTER_HPP_
#define SEMPR_ENTITY_SPATIAL_PROJECTIONFILTER_HPP_

//#include <ogr_spatialref.h>
#include <geos/geom/CoordinateFilter.h>
#include <geos/geom/Coordinate.h>
#include <GeographicLib/UTMUPS.hpp>             
#include <GeographicLib/MGRS.hpp>               //MGRS
#include <GeographicLib/TransverseMercator.hpp> //UTM
#include <GeographicLib/PolarStereographic.hpp> //UPS

#include <exception>
#include <string>

// weeeeeell... it's not an entity. more like... util?
namespace sempr {

namespace geom = geos::geom;

class ProjectionException : public std::exception
{
    private:
    std::string message_;

    public:
    explicit ProjectionException(const std::string &m) : message_(m) {}
    virtual const char *what() const throw()
    {
        return message_.c_str();
    }
};

class ProjectionZoneMissmatch : public std::exception
{
    private:
    std::string message_;

    public:
    explicit ProjectionZoneMissmatch(const std::string &m) : message_(m) {}
    virtual const char *what() const throw()
    {
        return message_.c_str();
    }
};

    
/// UTM Projection Filter
class UTMFilter : public geom::CoordinateFilter
{
public:
    virtual void filter_rw(geom::Coordinate* coordinate) const;

    void filter_ro(const geom::Coordinate* coordinate);

protected:
    UTMFilter(double a, double f, double k0, int zone);

    /// ref to const transform given on initialization
    const GeographicLib::TransverseMercator tm_;
    const int zone_;

    static double CentralMeridian(int zone);
};

/// Projection from lat/lon to UTM x/y
class UTMForwardFilter : public UTMFilter
{
public:
    UTMForwardFilter(double a, double f, double k0, int zone);

    void filter_rw(geom::Coordinate* coordinate) const;

};

/// Reverse Projection from x/y to UTM lat/lon
class UTMReversFilter : public UTMFilter
{
public:
    UTMReversFilter(double a, double f, double k0, int zone);

    void filter_rw(geom::Coordinate* coordinate) const;
};


/// UPS Projection Filter
class UPSFilter : public geom::CoordinateFilter
{
public:
    virtual void filter_rw(geom::Coordinate* coordinate) const;

    void filter_ro(const geom::Coordinate* coordinate);

protected:
    UPSFilter(double a, double f, double k0, bool north);

    /// ref to const transform given on initialization
    const GeographicLib::PolarStereographic ps_;
    const bool north_;
};

/// Projection from lat/lon to UTM x/y
class UPSForwardFilter : public UPSFilter
{
public:
    UPSForwardFilter(double a, double f, double k0, bool north);

    void filter_rw(geom::Coordinate* coordinate) const;
};

/// Reverse Projection from x/y to UTM lat/lon
class UPSReversFilter : public UPSFilter
{
public:
    UPSReversFilter(double a, double f, double k0, bool north);

    void filter_rw(geom::Coordinate* coordinate) const;
};

/*
class MGRSFilter : public geom::CoordinateFilter
{
public:
    MGRSFilter(int zone, bool north, int prec = 8);
}
*/

} /* sempr */


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_PROJECTIONFILTER_HPP_ */
