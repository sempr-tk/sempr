#ifndef SEMPR_UTIL_GENERICCOORDINATEFILTER_HPP_
#define SEMPR_UTIL_GENERICCOORDINATEFILTER_HPP_

#include <geos/geom/CoordinateFilter.h>
#include <geos/geom/Coordinate.h>

#include <functional>

namespace sempr {

/**
    A utility class that accepts any functional that works on a Coordinate* and
    thus makes it applicable to any geos::geom::Geometry.
*/
class GenericCoordinateFilter : public geos::geom::CoordinateFilter {
public:
    typedef std::function<void(geos::geom::Coordinate*)> rw_func_t;
    typedef std::function<void(const geos::geom::Coordinate*)> ro_func_t;

    GenericCoordinateFilter(rw_func_t fun);
    GenericCoordinateFilter(ro_func_t fun);

    void filter_rw(geos::geom::Coordinate*) const override;
    void filter_ro(const geos::geom::Coordinate*) override;

private:
    rw_func_t rwFun_;
    ro_func_t roFun_;
};

}

#endif /* include guard: SEMPR_UTIL_GENERICCOORDINATEFILTER_HPP_ */
