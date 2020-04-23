#include "util/GenericCoordinateFilter.hpp"

namespace sempr {

GenericCoordinateFilter::GenericCoordinateFilter(rw_func_t fun)
    : rwFun_(fun)
{
}

GenericCoordinateFilter::GenericCoordinateFilter(ro_func_t fun)
    : roFun_(fun)
{
}


void GenericCoordinateFilter::filter_rw(geos::geom::Coordinate* coordinate) const
{
    if (rwFun_)
    {
        rwFun_(coordinate);
    }
    else if (roFun_)
    {
        roFun_(coordinate);
    }
}

void GenericCoordinateFilter::filter_ro(const geos::geom::Coordinate* coordinate)
{
    if (!roFun_) throw std::exception();
    roFun_(coordinate);
}

}
