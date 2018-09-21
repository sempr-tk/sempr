#ifndef SEMPR_QUERY_SPATIALQUERY_HPP_
#define SEMPR_QUERY_SPATIALQUERY_HPP_

#include <sempr/query/Query.hpp>
#include <sempr/processing/SpatialIndexBase.hpp>
#include <sempr/entity/spatial/Geometry.hpp>
#include <sempr/entity/spatial/MultiPoint.hpp>
#include <vector>
#include <set>

#include <Eigen/Geometry>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

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
template<std::size_t dim = 3>
class SpatialIndexQuery : public Query, public core::OType< SpatialIndexQuery<dim> > {
public:
    typedef processing::SpatialQueryType SpatialQueryType;
    typedef processing::SpatialIndexBase<dim> SpatialIndexBase;
    typedef typename SpatialIndexBase::Box Box;
    typedef typename SpatialIndexBase::Point Point;
    typedef typename SpatialIndexBase::ValuePair ValuePair;
    typedef typename SpatialIndexBase::Vector Vector;

    using Ptr = std::shared_ptr< SpatialIndexQuery<dim> >;
    ~SpatialIndexQuery() {}

    std::string type() const override { return "SpatialIndexQuery" + std::to_string(dim) + "D"; }

    /** The set of geometries matching the criterium */
    std::set<entity::Geometry::Ptr> results;

    /** gets the reference bounding box and geometry pair. It could either be 2D or 3D. The geometry will be null if its only a box based query. */
    ValuePair refBoxGeometryPair() { return refPair_; }

    entity::SpatialReference::Ptr refCS() { return referenceCS_; }

    /** returns the mode of the query: WITHIN, NOT_WITHIN, ... */
    SpatialQueryType mode() const { return qtype_; }

    /** set the mode of the query */
    void mode(SpatialQueryType m) { qtype_ = m; }

    /** inverts the mode: WITHIN <-> NOTWITHIN etc. */
    void invert()
    {
        if (qtype_ == SpatialQueryType::NEAREST ) return;
    /*  inversion of query type:
        positive (WITHIN, CONTAINS, INTERSECTS) are even (0, 2, 4),
        negative (NOT_WITHIN, ...) are odd (1, 3, 5).
        Just increment even and decrement odd values.    */
        qtype_ = (int(qtype_) % 2 == 0) ? SpatialQueryType(int(qtype_)+1) : SpatialQueryType(int(qtype_)-1);
    }

    /**
        Query for everything within the bounding box of 'geometry'. Passes the Envelope3D of
        the geometry to 'withinBox'.
    */
    static SpatialIndexQuery::Ptr withinBoxOf(entity::Geometry::Ptr geometry)
    {
        return SpatialIndexQuery::createBoxQuery(geometry, SpatialQueryType::WITHIN);
    }

    static SpatialIndexQuery::Ptr containsBoxOf(entity::Geometry::Ptr geometry)
    {
        return SpatialIndexQuery::createBoxQuery(geometry, SpatialQueryType::CONTAINS);
    }

    static SpatialIndexQuery::Ptr intersectsBoxOf(entity::Geometry::Ptr geometry)
    {
        return SpatialIndexQuery::createBoxQuery(geometry, SpatialQueryType::INTERSECTS);
    }

    static SpatialIndexQuery::Ptr nearestToBoxOf(entity::Geometry::Ptr geometry)
    {
        return SpatialIndexQuery::createBoxQuery(geometry, SpatialQueryType::NEAREST);
    }

    static SpatialIndexQuery::Ptr coveredByBoxOf(entity::Geometry::Ptr geometry)
    {
        return SpatialIndexQuery::createBoxQuery(geometry, SpatialQueryType::COVERED);
    }

    static SpatialIndexQuery::Ptr overlapsBoxOf(entity::Geometry::Ptr geometry)
    {
        return SpatialIndexQuery::createBoxQuery(geometry, SpatialQueryType::COVERED);
    }


    /**
        Query for everything within the bbox specified. Explicit coordinate system.
        This creates a new GeometryCollection that is used for the query.
     */
    static SpatialIndexQuery::Ptr withinBox(const Vector& lower, const Vector& upper, entity::SpatialReference::Ptr cs)
    {
        return SpatialIndexQuery::createBoxQuery(lower, upper, cs, SpatialQueryType::WITHIN);
    }

    static SpatialIndexQuery::Ptr containsBox(const Vector& lower, const Vector& upper, entity::SpatialReference::Ptr cs)
    {
        return SpatialIndexQuery::createBoxQuery(lower, upper, cs, SpatialQueryType::CONTAINS);
    }

    static SpatialIndexQuery::Ptr intersectsBox(const Vector& lower, const Vector& upper, entity::SpatialReference::Ptr cs)
    {
        return SpatialIndexQuery::createBoxQuery(lower, upper, cs, SpatialQueryType::INTERSECTS);
    }

    static SpatialIndexQuery::Ptr nearestBox(const Vector& lower, const Vector& upper, entity::SpatialReference::Ptr cs)
    {
        return SpatialIndexQuery::createBoxQuery(lower, upper, cs, SpatialQueryType::NEAREST);
    }

    static SpatialIndexQuery::Ptr coveredByBox(const Vector& lower, const Vector& upper, entity::SpatialReference::Ptr cs)
    {
        return SpatialIndexQuery::createBoxQuery(lower, upper, cs, SpatialQueryType::COVERED);
    }

    static SpatialIndexQuery::Ptr overlapsBox(const Vector& lower, const Vector& upper, entity::SpatialReference::Ptr cs)
    {
        return SpatialIndexQuery::createBoxQuery(lower, upper, cs, SpatialQueryType::OVERLAPS);
    }

    // TODO Query for a specific geometry relations not only Boxes!
private:

    // the actual type
    SpatialQueryType qtype_;

    // the reference pair of bounding box and geometry
    ValuePair refPair_;

    // the reference coordinate system of this query. (same as geometry reference if geometry is not null)
    entity::SpatialReference::Ptr referenceCS_;

    // use static methods to construct queries
    SpatialIndexQuery() {}

    void setupPair(const Vector& lower, const Vector& upper, entity::Geometry::Ptr geo, entity::SpatialReference::Ptr cs)
    {
        ValuePair pair;
        pair.first = createBox(lower, upper);
        pair.second = geo;

        this->refPair_ = pair;
        this->referenceCS_ = cs;
    }

    Box createBox(const Vector& min, const Vector& max)
    {
        auto minP = SpatialIndexBase::toPoint(min);
        auto maxP = SpatialIndexBase::toPoint(max);
        return Box(minP, maxP);
    }

    // helper: create query from geo or upper/lower and a type
    static SpatialIndexQuery::Ptr createBoxQuery(entity::Geometry::Ptr geometry, SpatialQueryType type)
    {
        geos::geom::Coordinate min, max;
        geometry->findEnvelope(min, max);

        auto lower = SpatialIndexBase::toEigen(min);
        auto upper = SpatialIndexBase::toEigen(max);

        return createBoxQuery(lower, upper, geometry->getCS(), type);
    }

    static SpatialIndexQuery::Ptr createBoxQuery(const Vector& min, const Vector& max,
                                        entity::SpatialReference::Ptr cs, SpatialQueryType type)
    {
         if (!cs) return SpatialIndexQuery::Ptr();

        SpatialIndexQuery::Ptr query(new SpatialIndexQuery());
        query->setupPair(min, max, nullptr, cs);
        query->mode(type);
        return query;
    }

};

typedef SpatialIndexQuery<2> SpatialIndexQuery2D;
typedef SpatialIndexQuery<3> SpatialIndexQuery3D;

}}

#endif /* end of include guard SEMPR_QUERY_SPATIALQUERY_HPP_ */
