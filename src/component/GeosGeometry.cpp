#include <geos/geom/GeometryFactory.h>

#include "component/GeosGeometry.hpp"

namespace sempr {

GeosGeometry::GeosGeometry(geos::geom::Geometry* geometry)
    : geometry_(geometry)
{
}

GeosGeometry::GeosGeometry(std::unique_ptr<geos::geom::Geometry> geometry)
    : geometry_(std::move(geometry))
{
}

GeosGeometry::GeosGeometry()
    : geometry_(geos::geom::GeometryFactory::getDefaultInstance()->createEmptyGeometry())
{
}

GeosGeometry::~GeosGeometry()
{
    //geos::geom::GeometryFactory::getDefaultInstance()->destroyGeometry(geometry_.get());
    // will be cleaned up by the unique ptr
}

geos::geom::Geometry* GeosGeometry::geometry()
{
    return geometry_.get();
}

const geos::geom::Geometry* GeosGeometry::geometry() const
{
    return geometry_.get();
}

void GeosGeometry::setGeometry(geos::geom::Geometry* geometry)
{
    setGeometry(std::unique_ptr<geos::geom::Geometry>(geometry));
}

void GeosGeometry::setGeometry(std::unique_ptr<geos::geom::Geometry> geometry)
{
    /*
    if (geometry_) //will be cleaned up by the unique ptr!
        geos::geom::GeometryFactory::getDefaultInstance()->destroyGeometry(geometry_.get());
    */
    geometry_ = std::move(geometry);
}

} // ns sempr


namespace rete { namespace util {

template <>
std::string to_string(const sempr::GeosGeometryInterface& c)
{
    return std::string("GeosGeometry - ") + (c.geometry() ? c.geometry()->getGeometryType() : "nullptr");
}

template <>
std::string to_string(const sempr::GeosGeometryInterface::Ptr& c)
{
    if (!c) return "nullptr";
    return to_string(*c);
}

}}

