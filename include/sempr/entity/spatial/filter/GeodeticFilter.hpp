#ifndef SEMPR_ENTITY_SPATIAL_CEOCENTRICFILTER_HPP_
#define SEMPR_ENTITY_SPATIAL_CEOCENTRICFILTER_HPP_

//#include <ogr_spatialref.h>
#include <geos/geom/CoordinateFilter.h>
#include <geos/geom/Coordinate.h>
#include <GeographicLib/Geocentric.hpp> 

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


} /* sempr */


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_CEOCENTRICFILTER_HPP_ */
