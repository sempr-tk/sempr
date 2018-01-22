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

private:
    friend class odb::access;
    #pragma db type("TEXT")
    OGRLineString* geometry_;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LINESTRING_HPP_ */
