#include <GlobalCS_odb.h>
#include <sempr/entity/spatial/GlobalCS.hpp>

#include <string>
#include <typeinfo>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(GlobalCS)

GlobalCS::GlobalCS()
{
    // nothing to do.
}

GlobalCS::GlobalCS(const core::IDGenBase* idgen, GeodeticReference reference)
    : SpatialReference(idgen)
{
    initParameter(reference);
}

GlobalCS::~GlobalCS()
{
}

/*
std::shared_ptr<OGRCoordinateTransformation> GlobalCS::to(GlobalCS::Ptr other)
{
    auto raw = OGRCreateCoordinateTransformation(&frame_, &(other->frame_));
    auto t = std::shared_ptr<OGRCoordinateTransformation>(raw, OGRCoordinateTransformation::DestroyCT);
    return t;
}
 */


SpatialReference::Ptr GlobalCS::getRoot()
{
    // GlobalCS is always a root
    return this->shared_from_base<SpatialReference>();
}

Eigen::Affine3d GlobalCS::transformationToRoot() const
{
    return Eigen::Affine3d::Identity();
}

Eigen::Affine3d GlobalCS::transformationFromRoot() const
{
    return Eigen::Affine3d::Identity();
}

double GlobalCS::a()
{
    return a_;
}

double GlobalCS::f()
{
    return f_;
}

bool GlobalCS::sameReference(const GlobalCS::Ptr other) const
{
    return this->reference_ == other->reference_;
}

std::shared_ptr<geos::geom::CoordinateFilter> GlobalCS::to(const GlobalCS::Ptr other)
{
    //no single transformatin of the reference is not the same and representation is different!

/*
    auto thisType = typeid(*this);
    auto otherType = typeid(*other);
    bool sameReference = this->sameReference(other);

    if (thisType != otherType)
    {
        // tranformation between different types
        if(sameReference)
        {

        }
        else
        {
            // could not change the reference and transform it in one step!
        }
    }
    else
    {
        // same type
        if (!sameReference)
        {
            // reference transformatin
        }
        else
        {
            // nothing to do!
        }
    }*/

}


void GlobalCS::initParameter(GeodeticReference reference)
{
    switch(reference)
    {
        case GeodeticReference::WGS84: 
        case GeodeticReference::EPSG4326:
        {
            a_ = 6378137;
            f_ = 1/298.257223563;
            break;
        }
        case GeodeticReference::GRS80:
        case GeodeticReference::NAD83:
        {
            a_ = 6378137;
            f_ = 1/298.257222101;
            break;
        }
        case GeodeticReference::NAD27:
        {
            a_ = 6378206.4;
            f_ = 1/294.978698214;
            break;
        }
        case GeodeticReference::IERS03:
        {
            a_ = 6378136.6;
            f_ = 1/298.25642;
            break;
        }
        case GeodeticReference::HAYFORD:
        case GeodeticReference::ED50:
        {
            a_ = 6378388;
            f_ = 1/297;
            break;
        }
    }

    reference_ = reference;
}



}}
