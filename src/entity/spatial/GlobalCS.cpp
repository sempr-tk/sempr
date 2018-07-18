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

GlobalCS::GlobalCS(const core::IDGenBase* idgen)
    : SpatialReference(idgen)
{
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


}}
