#ifndef SEMPR_ENTITY_SPATIAL_GEODETICFILTER_HPP_
#define SEMPR_ENTITY_SPATIAL_GEODETICFILTER_HPP_

//#include <ogr_spatialref.h>
#include <geos/geom/CoordinateSequenceFilter.h>
#include <geos/geom/Coordinate.h>

// weeeeeell... it's not an entity. more like... util?
namespace sempr {

namespace geom = geos::geom;

    /** Exception that may be thrown during transformToCS. */
class GeodeticException : public std::exception
{
    private:
    std::string message_;

    public:
    explicit GeodeticException(const std::string &m) : message_(m) {}
    virtual const char *what() const throw()
    {
        return message_.c_str();
    }
};


class EmptyFilter : public geom::CoordinateSequenceFilter 
{
public:
    /**
        Create an empty filter.
    */
    EmptyFilter() {};

    void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override {};

    void filter_ro(const geom::CoordinateSequence& seq, std::size_t i) override {};

    bool isDone() const override {return true;};

    bool isGeometryChanged() const override {return false;};

};



} /* sempr */


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEODETICFILTER_HPP_ */
