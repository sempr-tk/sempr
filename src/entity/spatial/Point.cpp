#include <sempr/entity/spatial/Point.hpp>
#include <Point_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(Point)

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
    geometry_ = factory_->createPoint();
}


Point::~Point()
{
    factory_->destroyGeometry(geometry_);
    geometry_ = nullptr;
}


const geom::Point* Point::geometry() 
{
    return dynamic_cast<geom::Point*>(geometry_);
}

void Point::setGeometry(geom::Point* point)
{
    geometry_ = point;
}

Point::Ptr Point::clone() const 
{
    // raw clone is virtual! :)
    return Point::Ptr(raw_clone());
}

Point* Point::raw_clone() const
{
    Point* newInstance = new Point();
    // set the same reference frame
    newInstance->setCS(this->getCS());

    // copy the geometry
    newInstance->geometry_ = geometry_->clone();

    return newInstance;
}

}}
