#ifndef SEMPR_ENTITY_AGROENTITY_HPP
#define SEMPR_ENTITY_AGROENTITY_HPP

#include <sempr/entity/Entity.hpp>
#include <sempr/entity/PointCloud.hpp>
#include <ogr_geometry.h>

#include <climits>

#include <iostream>


#include <type_traits>


namespace sempr { namespace entity { namespace agro {

#pragma db object
class AgroEntity : public Entity
{
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<AgroEntity>;

    AgroEntity();
    AgroEntity(const sempr::core::IDGenBase*);
    virtual ~AgroEntity();

    OGRPolygon* geometry() { return geometry_; }

    PointCloud::BoundingBox boundingBox();
    // This should be put into the Module?
    //bool calculateIntersection(const Polygon::Ptr& polygon);

protected:
    friend class odb::access;
    #pragma db type("TEXT")
    OGRPolygon* geometry_;

    // I could use the OGRGeometry->Boundary() function,
    // but this will create a new OGRGeometry
    // and I would still need to iterate over the Points, to get the raw coordinates of the BB
    // on the other hand, I could just use the OGRGeometry-Intersects() on the created
    // OGRGeometry ... is there any usecase for the min/max x,y,z coordinates other then intersection?
};

}}}

#endif // SEMPR_ENTITY_AGROENTITY_HPP
