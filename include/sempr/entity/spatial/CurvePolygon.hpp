#ifndef SEMPR_ENTITY_SPATIAL_CURVEPOLYGON_HPP_
#define SEMPR_ENTITY_SPATIAL_CURVEPOLYGON_HPP_

#include <sempr/entity/spatial/Geometry.hpp>

namespace sempr { namespace entity {

/**
    Container class for OGRCurvePolygon
*/
#pragma db object
class CurvePolygon : public Geometry {
public:
    using Ptr = std::shared_ptr<CurvePolygon>;
    ENTITY_DEFAULT_EVENT_METHODS(CurvePolygon, Geometry);

    CurvePolygon();
    CurvePolygon(const core::IDGenBase*);
    virtual ~CurvePolygon();

    OGRCurvePolygon* geometry() override;

private:
    friend class odb::access;
    #pragma db type("TEXT")
    OGRCurvePolygon* geometry_;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_CURVEPOLYGON_HPP_ */
