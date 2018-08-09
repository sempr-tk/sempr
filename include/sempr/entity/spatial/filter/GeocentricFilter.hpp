#ifndef SEMPR_ENTITY_SPATIAL_GEOCENTRICFILTER_HPP_
#define SEMPR_ENTITY_SPATIAL_GEOCENTRICFILTER_HPP_

#include <cstddef>
#include <exception>
#include <string>

#include <geos/geom/CoordinateSequenceFilter.h>
#include <geos/geom/CoordinateSequence.h>

#include <GeographicLib/Geocentric.hpp>
#include <GeographicLib/LocalCartesian.hpp>


// weeeeeell... it's not an entity. more like... util?
namespace sempr {

namespace geom = geos::geom;

class GeocentricException : public std::exception
{
    private:
    std::string message_;

    public:
    explicit GeocentricException(const std::string &m) : message_(m) {}
    virtual const char *what() const throw()
    {
        return message_.c_str();
    }
};


class ECEFFilter : public geom::CoordinateSequenceFilter
{
public:
    virtual void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;

    void filter_ro(const geom::CoordinateSequence& seq, std::size_t i) override;

    bool isDone() const override;

    bool isGeometryChanged() const override;
protected:
    ECEFFilter(double a, double f);

    const GeographicLib::Geocentric ecef_;

    bool changed_;
};

class ECEFForwardFilter : public ECEFFilter
{
public:
    ECEFForwardFilter(double a, double f);

    void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;
};

class ECEFReverseFilter : public ECEFFilter
{
public:
    ECEFReverseFilter(double a, double f);

    void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;
};


class LTGFilter : public geom::CoordinateSequenceFilter
{
public:
    virtual void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;

    void filter_ro(const geom::CoordinateSequence& seq, std::size_t i) override;

    bool isDone() const override;

    bool isGeometryChanged() const override;
protected:
    LTGFilter(double lat0, double lon0, double h0, double a, double f);

    const GeographicLib::LocalCartesian ltg_;

    bool changed_;
};

class LTGForwardFilter : public LTGFilter
{
public:
    LTGForwardFilter(double lat0, double lon0, double h0, double a, double f);

    void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;
};

class LTGReverseFilter : public LTGFilter
{
public:
    LTGReverseFilter(double lat0, double lon0, double h0, double a, double f);

    void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;
};


} /* sempr */


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEOCENTRICFILTER_HPP_ */
