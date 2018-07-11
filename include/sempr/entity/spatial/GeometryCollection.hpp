#ifndef SEMPR_ENTITY_SPATIAL_GEOCOLLECTION_HPP_
#define SEMPR_ENTITY_SPATIAL_GEOCOLLECTION_HPP_

#include <sempr/entity/spatial/Geometry.hpp>
#include <geos/geom/GeometryCollection.h>

namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
    Container class for the GEOM GeometryCollection
*/
#pragma db object
class GeometryCollection : public Geometry {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<GeometryCollection>;

    GeometryCollection();
    GeometryCollection(const core::IDGenBase*);
    virtual ~GeometryCollection();

    geom::GeometryCollection* geometry() override;

    void setCollection(const std::vector<geom::Geometry*> geoms);

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    GeometryCollection::Ptr clone() const;

private:
    friend class odb::access;

    #pragma db type("TEXT")
    geom::GeometryCollection* geometry_;

    virtual GeometryCollection* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEOCOLLECTION_HPP_ */
