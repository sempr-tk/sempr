#include <geos/geom/GeometryFactory.h>

#include "component/GeosGeometry.hpp"

namespace sempr {

GeosGeometry::GeosGeometry(geos::geom::Geometry* geometry)
    : geometry_(geometry)
{
}

GeosGeometry::GeosGeometry()
    : geometry_(geos::geom::GeometryFactory::getDefaultInstance()->createEmptyGeometry())
{
}

GeosGeometry::~GeosGeometry()
{
    geos::geom::GeometryFactory::getDefaultInstance()->destroyGeometry(geometry_);
}

geos::geom::Geometry* GeosGeometry::geometry()
{
    return geometry_;
}

const geos::geom::Geometry* GeosGeometry::geometry() const
{
    return geometry_;
}

void GeosGeometry::setGeometry(geos::geom::Geometry* geometry)
{
    if (geometry_)
        geos::geom::GeometryFactory::getDefaultInstance()->destroyGeometry(geometry_);
    geometry_ = geometry;
}

}
