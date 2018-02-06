#ifndef SEMPR_ENTITY_SPATIAL_POLYGON_HPP_
#define SEMPR_ENTITY_SPATIAL_POLYGON_HPP_

#include <sempr/entity/spatial/CurvePolygon.hpp>

namespace sempr { namespace entity {

/**
    Container class for OGRPolygon
*/
#pragma db object
class Polygon : public CurvePolygon {
public:
    using Ptr = std::shared_ptr<Polygon>;
    ENTITY_DEFAULT_EVENT_METHODS(Polygon, CurvePolygon);

    Polygon();
    Polygon(const core::IDGenBase*);
    virtual ~Polygon();

    OGRPolygon* geometry() override;

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    Polygon::Ptr clone() const;

private:
    friend class odb::access;
    #pragma db type("TEXT")
    OGRPolygon* geometry_;

    virtual Polygon* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_POLYGON_HPP_ */
