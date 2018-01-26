#ifndef SEMPR_ENTITY_SPATIAL_SPATIALREFERENCE_HPP_
#define SEMPR_ENTITY_SPATIAL_SPATIALREFERENCE_HPP_

#include <Eigen/Geometry>
#include <sempr/entity/Entity.hpp>

namespace sempr { namespace entity {

/**
    This class represents spatial reference systems. It is used to describe relations between
    geometries: Every geometry is bound to a SpatialReference, and every SpatialReference is either
    1. a geographic reference system, which is purely lat/lon and cannot be chained
    2. a projection reference system, which defines a x/y/z coordinate system at a specific location
       on the globe
    3. a local reference system, which defines an affine transformation to another local reference
       system or a projection reference system.
*/
#pragma db object
class SpatialReference : public Entity {
public:
    using Ptr = std::shared_ptr<SpatialReference>;

    /// get the root of this reference system (not the parent!)
    /// not const -- might return "shared_from_this()"...
    virtual SpatialReference::Ptr getRoot();
    /// get the transformation to the root of this reference system
    virtual Eigen::Affine3d transformationToRoot() const;
    /// get the transformation from the root to this reference system
    virtual Eigen::Affine3d transformationFromRoot() const;

    virtual ~SpatialReference() {}

protected:
    SpatialReference();
    SpatialReference(const core::IDGenBase*);
    friend class odb::access;

};




}}


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_SPATIALREFERENCE_HPP_ */
