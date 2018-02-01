#include <sempr/entity/spatial/Point.hpp>
#include <Point_odb.h>

namespace sempr { namespace entity {

Point::Point() : Point(new core::IDGen<Point>())
{
}

Point::Point(const core::IDGenBase* idgen)
    : Geometry(idgen)
{
    this->setDiscriminator<Point>();
    /**
        I think we can indeed create geometry objects through their ctor, BUT:
        To load an object from wkt/wkb we use the GeometryFactory, and it is explicitely stated
        that a geometry created through the factory should be destroyed by it, too. In order to not
        need to differ between the method of creation just use the factory.

        -- But: We will need to cast towards a more specific type, which is kind of ugly.
        The OGR documentation states that:
            void OGRGeometryFactory::destroyGeometry( OGRGeometry* poGeom )
                Destroy geometry object.
                Equivalent to invoking delete on a geometry, but it guaranteed to take place within
                the context of the GDAL/OGR heap.
        Is a possible heap-corruption just a windows-problem? I've never heard of that before now.
    */
    geometry_ = static_cast<OGRPoint*>(OGRGeometryFactory::createGeometry(wkbPoint));
}


Point::~Point()
{
    OGRGeometryFactory::destroyGeometry(geometry_);
}


OGRPoint* Point::geometry() {
    return geometry_;
}

}}
