#include <sempr/entity/spatial/SpatialObject.hpp>
#include <SpatialObject_odb.h>

namespace sempr { namespace entity {

SpatialObject::SpatialObject()
    : Entity(new core::IDGen<SpatialObject>())
{
    geometry_.reset(new GeometryCollection());
    property_.reset(new RDFPropertyMap(*this));

    this->registerChildEntity(geometry_);
    this->registerChildEntity(property_);
}

GeometryCollection::Ptr SpatialObject::geometry()
{
    return geometry_;
}

RDFPropertyMap::Ptr SpatialObject::properties()
{
    return property_;
}

std::string SpatialObject::type(double* confidence) const
{
    double maxConf = std::numeric_limits<double>::lowest();
    std::string bestType = "";
    for (auto kv : types_)
    {
        if (kv.second > maxConf){
            maxConf = kv.second;
            bestType = kv.first;
        }
    }

    if (confidence) *confidence = maxConf;
    return bestType;
}

void SpatialObject::type(const std::string& type, double confidence)
{
    types_[type] = confidence;
}


}}
