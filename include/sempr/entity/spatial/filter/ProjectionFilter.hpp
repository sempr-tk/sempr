#ifndef SEMPR_ENTITY_SPATIAL_PROJECTIONFILTER_HPP_
#define SEMPR_ENTITY_SPATIAL_PROJECTIONFILTER_HPP_

#include <cstddef>
#include <exception>
#include <string>

#include <geos/geom/CoordinateSequenceFilter.h>
#include <geos/geom/CoordinateSequence.h>

#include <geos/geom/CoordinateFilter.h>
#include <geos/geom/Coordinate.h>

#include <GeographicLib/UTMUPS.hpp>             //Util
#include <GeographicLib/MGRS.hpp>               //MGRS
#include <GeographicLib/TransverseMercator.hpp> //UTM
#include <GeographicLib/PolarStereographic.hpp> //UPS

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
class UTMFilter : public geom::CoordinateSequenceFilter
{
public:
    virtual void filter_rw(geom::CoordinateSequence& seq, std::size_t i);

    void filter_ro(const geom::CoordinateSequence& seq, std::size_t i) override;

    bool isDone() const;

    bool isGeometryChanged() const;

protected:
    UTMFilter(double a, double f, double k0, int zone, bool north);

    static double centralMeridian(int zone);

    /// ref to const transform given on initialization
    const GeographicLib::TransverseMercator tm_;
    const int zone_;

    const double falseeasting_, falsenorthing_;

    bool done_;
    bool changed_;
};

/// Projection from lat/lon to UTM x/y
class UTMForwardFilter : public UTMFilter
{
public:
    UTMForwardFilter(double a, double f, double k0, int zone, bool north);

    void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;

};

/// Reverse Projection from x/y to UTM lat/lon
class UTMReversFilter : public UTMFilter
{
public:
    UTMReversFilter(double a, double f, double k0, int zone, bool north);

    void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;
};


/// UPS Projection Filter
class UPSFilter : public geom::CoordinateSequenceFilter
{
public:
    virtual void filter_rw(geom::CoordinateSequence& seq, std::size_t i);

    void filter_ro(const geom::CoordinateSequence& seq, std::size_t i) override;

    bool isDone() const;

    bool isGeometryChanged() const;

protected:
    UPSFilter(double a, double f, double k0, bool north);

    /// ref to const transform given on initialization
    const GeographicLib::PolarStereographic ps_;
    const bool north_;

    bool done_;
    bool changed_;
};

/// Projection from lat/lon to UTM x/y
class UPSForwardFilter : public UPSFilter
{
public:
    UPSForwardFilter(double a, double f, double k0, bool north);

    void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;
};

/// Reverse Projection from x/y to UTM lat/lon
class UPSReversFilter : public UPSFilter
{
public:
    UPSReversFilter(double a, double f, double k0, bool north);

    void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;
};


class MGRSFilter : public geom::CoordinateSequenceFilter
{
public:
    virtual void filter_rw(geom::CoordinateSequence& seq, std::size_t i);

    void filter_ro(const geom::CoordinateSequence& seq, std::size_t i) override;

    bool isDone() const;

    bool isGeometryChanged() const;

protected:
    MGRSFilter(const std::string& GZDSquareID);   //only WGS84!

    /// ref to const transform given on initialization
    const std::string GZDSquareID_;

    bool done_;
    bool changed_;
};

/// Projection from lat/lon to UTM x/y
class MGRSForwardFilter : public MGRSFilter
{
public:
    MGRSForwardFilter(const std::string& GZDSquareID);

    void filter_rw(geom::CoordinateSequence& seq, std::size_t idx) override;
};

/// Reverse Projection from x/y to UTM lat/lon
class MGRSReversFilter : public MGRSFilter
{
public:
    MGRSReversFilter(const std::string& GZDSquareID);

    void filter_rw(geom::CoordinateSequence& seq, std::size_t idx) override;
};

} /* sempr */


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_PROJECTIONFILTER_HPP_ */
