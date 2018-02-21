#ifndef SEMPR_ENTITY_SPATIAL_CURVE_HPP_
#define SEMPR_ENTITY_SPATIAL_CURVE_HPP_

#include <sempr/entity/spatial/Geometry.hpp>

namespace sempr { namespace entity {

/**
    Container class for OGRCurve
*/
#pragma db object
class Curve : public Geometry {
public:
    using Ptr = std::shared_ptr<Curve>;
    ENTITY_DEFAULT_EVENT_METHODS(Curve, Geometry);

    Curve();
    Curve(const core::IDGenBase*);
    virtual ~Curve();

    OGRCurve* geometry() override;

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    Curve::Ptr clone() const;

private:
    friend class odb::access;
    // #pragma db type("TEXT")
    // OGRCurve* geometry_; // abstract!

    Curve* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_CURVE_HPP_ */
