#include <sempr/entity/spatial/SpatialObject.hpp>
#include <SpatialObject_odb.h>

#include <sempr/entity/RDFValue.hpp>

namespace sempr { namespace entity {

SpatialObject::SpatialObject()
    : Entity(new core::IDGen<SpatialObject>())
{
    this->setDiscriminator<SpatialObject>();
    
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

    // also update the rdf representation with the current highest type.
    std::string bestType = this->type();
    (*property_)("type", core::rdf::baseURI()) =  RDFResource("<" + bestType + ">");
    property_->changed();
}


unsigned int SpatialObject::lastSeen() const
{
    unsigned int ts = (*property_)["lastSeen"];
    return ts;
}

void SpatialObject::lastSeen(unsigned int ts)
{
    (*property_)["lastSeen"] = ts;
    property_->changed();
}

bool SpatialObject::vanished() const
{
    bool v = (*property_)["vanished"];
    return v;
}

void SpatialObject::vanished(bool v)
{
    (*property_)["vanished"] = v;
    property_->changed();
}

}}
