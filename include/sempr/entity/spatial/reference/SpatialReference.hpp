#ifndef SEMPR_ENTITY_SPATIAL_SPATIALREFERENCE_HPP_
#define SEMPR_ENTITY_SPATIAL_SPATIALREFERENCE_HPP_

#include <Eigen/Geometry>
#include <sempr/entity/Entity.hpp>

#include <geos/geom/CoordinateFilter.h>
#include <geos/geom/CoordinateSequenceFilter.h>

namespace sempr { namespace entity {

typedef geos::geom::CoordinateSequenceFilter Filter;
typedef std::shared_ptr<Filter> FilterPtr;
typedef std::vector<FilterPtr> FilterList;

/**
TODO geometry: methods to assign / change ref systems

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
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<SpatialReference>;

    /// get the root of this reference system (not the parent!)
    /// not const -- might return "shared_from_this()"...
    virtual SpatialReference::Ptr getRoot();

    /**
     * Check if this and the other cs are equal. (not the same!)
     * Note: This have to be override by derived class with attributes!
     */
    virtual bool isEqual(const SpatialReference::Ptr other) const;

    /**
        Check if this SpatialReference is the child of another one. This is usefull to check if
        we need to recompute e.g. a Geometry based on the change of some arbitrary frame, as
        currently no changed()-events are emitted for child-frames (or their connected geometries).
        Feel free to discuss this here: https://github.com/sempr-tk/sempr/issues/13
    */
    // we dont need a "parent()"-method to do this. Default impl returns false, and only the
    // LocalCS really needs to override this.
    virtual bool isChildOf(SpatialReference::Ptr other) const;

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
