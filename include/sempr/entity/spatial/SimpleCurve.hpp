#ifndef SEMPR_ENTITY_SPATIAL_SIMPLECURVE_HPP_
#define SEMPR_ENTITY_SPATIAL_SIMPLECURVE_HPP_

#include <sempr/entity/spatial/Curve.hpp>

namespace sempr { namespace entity {

/**
    Container class for OGRSimpleCurve
*/
#pragma db object
class SimpleCurve : public Curve {
public:
    using Ptr = std::shared_ptr<SimpleCurve>;
    ENTITY_DEFAULT_EVENT_METHODS(SimpleCurve, Curve);

    SimpleCurve();
    SimpleCurve(const core::IDGenBase*);
    virtual ~SimpleCurve();

    OGRSimpleCurve* geometry() override;

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    SimpleCurve::Ptr clone() const;

private:
    friend class odb::access;
    // #pragma db type("TEXT")
    // OGRSimpleCurve* geometry_;   // abstract

    SimpleCurve* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_SIMPLECURVE_HPP_ */
