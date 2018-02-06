#ifndef SEMPR_ENTITY_SPATIAL_LINESTRING_HPP_
#define SEMPR_ENTITY_SPATIAL_LINESTRING_HPP_

#include <sempr/entity/spatial/SimpleCurve.hpp>

namespace sempr { namespace entity {

/**
    Container class for OGRLineString
*/
#pragma db object
class LineString : public SimpleCurve {
public:
    using Ptr = std::shared_ptr<LineString>;
    ENTITY_DEFAULT_EVENT_METHODS(LineString, SimpleCurve);

    LineString();
    LineString(const core::IDGenBase*);
    virtual ~LineString();

    OGRLineString* geometry() override;

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    LineString::Ptr clone() const;

private:
    friend class odb::access;
    #pragma db type("TEXT")
    OGRLineString* geometry_;

    virtual LineString* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LINESTRING_HPP_ */
