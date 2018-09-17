#include <sempr/entity/spatial/PointCloud.hpp>
#include <PointCloud_odb.h>

#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(PointCloud)

PointCloud::PointCloud() : 
    PointCloud(new sempr::core::IDGen<PointCloud>())
{
}

PointCloud::PointCloud(const sempr::core::IDGenBase* idgen) : 
    Entity(idgen)
{
    setDiscriminator<PointCloud>();
    geometry_ = geom::GeometryFactory::getDefaultInstance()->createMultiPoint();
}

PointCloud::~PointCloud()
{
    if (geometry_)
    {
        geom::GeometryFactory::getDefaultInstance()->destroyGeometry(geometry_);
        geometry_ = nullptr;
    }
}

const geom::MultiPoint* PointCloud::getGeometry() const
{
    return geometry_;
}

geom::MultiPoint* PointCloud::getGeometryMut()
{
    return geometry_;
}

void PointCloud::setGeometry(geom::MultiPoint* geometry)
{
    if (geometry_)
        geom::GeometryFactory::getDefaultInstance()->destroyGeometry(geometry_);

    geometry_ = geometry;
}

void PointCloud::setPoints(const std::vector<geom::Coordinate>& coordinates)
{
    setGeometry(geom::GeometryFactory::getDefaultInstance()->createMultiPoint(coordinates));
}

PointCloud::Ptr PointCloud::clone() const 
{
    // raw clone is virtual! :)
    return PointCloud::Ptr(raw_clone());
}

PointCloud* PointCloud::raw_clone() const
{
    PointCloud* newInstance = new PointCloud();
    // set the same reference frame
    newInstance->setCS(this->getCS());

    // copy the geometry
    newInstance->setGeometry( dynamic_cast<geom::MultiPoint*>(geometry_->clone()) );
    
    return newInstance;
}

void PointCloud::setCS(SpatialReference::Ptr cs) 
{
    referenceFrame_ = cs;
}

SpatialReference::Ptr PointCloud::getCS() const 
{
    return referenceFrame_;
}



}}
