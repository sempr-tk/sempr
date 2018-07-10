#ifndef SEMPR_ENTITY_SPATIAL_POLYGON_HPP_
#define SEMPR_ENTITY_SPATIAL_POLYGON_HPP_

#include <sempr/entity/spatial/Geometry.hpp>
#include <geos/geom/Polygon.h>

namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
    Container class for GEOM Polygon
*/
#pragma db object
class Polygon : public Geometry {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Polygon>;

    Polygon();
    Polygon(const core::IDGenBase*);
    virtual ~Polygon();

    const geom::Polygon* geometry() override;

    void setGeometry(geom::Polygon* polygon);

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    Polygon::Ptr clone() const;

private:
    friend class odb::access;

    virtual Polygon* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_POLYGON_HPP_ */
