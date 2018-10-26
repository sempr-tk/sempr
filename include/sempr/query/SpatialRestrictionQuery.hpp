#ifndef SEMPR_QUERY_SPATIALRESTRICTIONQUERY_HPP_
#define SEMPR_QUERY_SPATIALRESTRICTIONQUERY_HPP_

#include "sempr/query/Query.hpp"

#include "sempr/entity/GeometricObject.hpp"

#include <vector>
#include <string>
#include <memory>

namespace sempr { namespace query {


enum OperationMode {
    INTERSECTION,
    UNION,
    DIFFERENCE,
    SYM_DIFFERENCE
};

class SpatialRestrictionQuery : public Query, public core::OType< SpatialRestrictionQuery > {
public:
    std::vector<entity::AbstractSpatialObject::Ptr> results;

    SpatialRestrictionQuery(const std::vector<std::string>& types, OperationMode mode, const std::vector<std::string>& restrictions) :
        reference_(nullptr),
        types_(types),
        mode_(mode),
        restrictions_(restrictions)
    { }


    SpatialRestrictionQuery(entity::AbstractSpatialObject::Ptr reference, OperationMode mode, const std::vector<std::string>& restrictions) :
        reference_(reference),
        mode_(mode),
        restrictions_(restrictions)
    { }

    ~SpatialRestrictionQuery() {}

    std::string type() const override { return "SpatialRestrictionQuery"; }

    const entity::AbstractSpatialObject::Ptr reference() const { return reference_; }
    const bool hasReference() const { return reference_ ? true : false; }
    const std::vector<std::string>& types() const { return types_; }
    const OperationMode mode() const { return mode_; }
    const std::vector<std::string>& restrictions() { return restrictions_; }

private:
    entity::AbstractSpatialObject::Ptr reference_;
    std::vector<std::string> types_;

    OperationMode mode_;

    std::vector<std::string> restrictions_;

};

}} //sempr::query

#endif //SEMPR_QUERY_SPATIALRESTRICTIONQUERY_HPP_