#include <sempr/entity/spatial/Polygon.hpp>
#include <Polygon_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(Polygon)

Polygon::Polygon() : Polygon(new core::IDGen<Polygon>())
{
}

Polygon::Polygon(const core::IDGenBase* idgen)
    : Geometry(idgen)
{
    this->setDiscriminator<Polygon>();

    geometry_ = factory_->createPolygon();
}

Polygon::~Polygon()
{
    factory_->destroyGeometry(geometry_);
    geometry_ = nullptr;
}

const geom::Polygon* Polygon::geometry() 
{
    return dynamic_cast<geom::Polygon*>(geometry_);
}

void Polygon::setGeometry(geom::Polygon* polygon)
{
    geometry_ = polygon;
}

Polygon::Ptr Polygon::clone() const 
{
    // raw clone is virtual! :)
    return Polygon::Ptr(raw_clone());
}

Polygon* Polygon::raw_clone() const
{
    Polygon* newInstance = new Polygon();
    // set the same reference frame
    newInstance->setCS(this->getCS());

    // copy the geometry
    newInstance->geometry_ = geometry_->clone(); // use OGRGeometry copy ctor
    return newInstance;
}

}}
