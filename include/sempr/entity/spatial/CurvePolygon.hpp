#ifndef SEMPR_ENTITY_SPATIAL_CURVEPOLYGON_HPP_
#define SEMPR_ENTITY_SPATIAL_CURVEPOLYGON_HPP_

#include <sempr/entity/spatial/Geometry.hpp>

namespace sempr { namespace entity {

/**
    Container class for OGRCurvePolygon
    TODO: Polygon derives from this but has its own geometry-ptr, how can we clean this up? (as is, Polygon has to geometry_ fields (though a private one)). So, creating a polygon also creates an always empty curvepolygon.
*/
#pragma db object
class CurvePolygon : public Geometry {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<CurvePolygon>;

    CurvePolygon();
    CurvePolygon(const core::IDGenBase*);
    virtual ~CurvePolygon();

    OGRCurvePolygon* geometry() override;

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    CurvePolygon::Ptr clone() const;

private:
    friend class odb::access;
    #pragma db type("TEXT")
    OGRCurvePolygon* geometry_;

    virtual CurvePolygon* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_CURVEPOLYGON_HPP_ */
