#ifndef SEMPR_QUERY_SPATIALQUERY_HPP_
#define SEMPR_QUERY_SPATIALQUERY_HPP_

#include <sempr/query/Query.hpp>
#include <sempr/entity/spatial/Geometry.hpp>
#include <vector>
#include <Eigen/Geometry>
#include <set>

namespace sempr { namespace query {


/**
    This class defines queries to the SpatialIndex, which can be from a subset of the boost
    spatial predicates. A query consists of a few different things:
        1. [x] a geometry which defines the region of interest
        2. [x] (obviously?) a coordinate system in which the above mentioned geometry lives
        3. [x] a spatial predicate, something like "within", "intersects", etc.
        4. [x] a set of results.
        (5. [ ] the template parameter can be used to select which type of geometries will be returned)

    It only provides an interface to the SpatialIndex but does not provide any further functionality
    beyond the comparison of bounding boxes. Any work/checks on concrete geometries must be done
    by the user or a specialized processing module.
*/
class SpatialIndexQuery : public Query {
public:
    using Ptr = std::shared_ptr<SpatialIndexQuery>;
    ~SpatialIndexQuery();

    std::string type() const override { return "SpatialIndexQuery"; }

    /** The set of geometries matching the criterium */
    std::set<entity::Geometry::Ptr> results;

    enum QueryType {
        WITHIN = 0, NOT_WITHIN,
        CONTAINS, NOT_CONTAINS,
        INTERSECTS, NOT_INTERSECTS
    };

    /** returns the mode of the query: WITHIN, NOT_WITHIN, ... */
    QueryType mode() const;
    /** set the mode of the query */
    void mode(QueryType m);

    /** get hold of the reference geometry (the 8 cornerpoints of the bounding box) */
    entity::Geometry::Ptr refGeo();

    /**
        Query for everything within the bounding box of 'geometry'. Passes the Envelope3D of
        the geometry to 'withinBox'.
    */
    static SpatialIndexQuery::Ptr withinBoxOf(entity::Geometry::Ptr geometry);

    /**
        Query for everything within the bbox specified. Explicit coordinate system.
        This creates a new GeometryCollection that is used for the query.
     */
    static SpatialIndexQuery::Ptr withinBox(const Eigen::Vector3d& lower, const Eigen::Vector3d& upper,
                                            entity::SpatialReference::Ptr cs);

    /**
        TODO: how to say "not within?"
        This would be nice:
            SpatialIndexQuery::Ptr q = !SpatialIndexQuery::within(geo);
        But we also want to get a SpatialIndexQuery::Ptr, for which an overloaded operator!() could
        break stuff. Implementing it only for SpatialIndexQuery (w/o Ptr) would lead to...
            SpatialIndexQuery::Ptr q = !(*SpatialIndexQuery::within(geo));
        So what about:
            SpatialIndexQuery::Ptr q = SpatialIndexQuery::within(geo);
            q->invert();
    */


    // TODO CONTAINS and INTERSECTS
private:
    // helper: construct collection of bbox-corner-points from a geometry

    // use static methods to construct queries
    SpatialIndexQuery();

    // the actual type
    QueryType qtype_;

    // the reference geometry
    entity::Geometry::Ptr refGeo_;
};

}}

#endif /* end of include guard SEMPR_QUERY_SPATIALQUERY_HPP_ */
