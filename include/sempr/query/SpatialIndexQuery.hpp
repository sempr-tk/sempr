#ifndef SEMPR_QUERY_SPATIALQUERY_HPP_
#define SEMPR_QUERY_SPATIALQUERY_HPP_

#include <sempr/query/Query.hpp>
#include <sempr/entity/spatial/Geometry.hpp>
#include <sempr/entity/spatial/MultiPoint.hpp>
#include <vector>
#include <set>

#include <Eigen/Geometry>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

// forward declaration of SpatialIndex      // ToDo: Move this to a own Header File!
namespace sempr { namespace processing {
    namespace bg = boost::geometry;
    namespace bgi = boost::geometry::index;

    template<std::size_t dim>
    struct SpatialIndexBase
    {
        /**
        Specify what is stored in the R-Tree:
            boxes, made out of points, consisting of 3 double, in cartesian space.
        NOTE: Boost seems to support geographic and spherical coordinates (lat-long etc) here, how
        does this affect the RTree? Can we use this to support indexing on lat-lon later on?
        */
        typedef bg::model::point<double, dim, bg::cs::cartesian> Point;
        typedef bg::model::box<Point> Box;
        typedef std::pair<Box, entity::Geometry::Ptr> ValuePair;  // pair af a bounding box and a translated geometry clone
        typedef bgi::rtree<ValuePair, bgi::quadratic<16> > RTree;
    };
}}

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
    typedef typename processing::SpatialIndexBase<dim>::ValuePair ValuePair;
    typedef typename processing::SpatialIndexBase<dim>::Box Box;
    typedef typename processing::SpatialIndexBase<dim>::Point Point;
    typedef Eigen::Matrix<double, dim, 1> EigenVector;

    using Ptr = std::shared_ptr< SpatialIndexQuery<dim> >;
    ~SpatialIndexQuery() {}

    std::string type() const override { return "SpatialIndexQuery" + std::to_string(dim) + "D"; }

    /** The set of geometries matching the criterium */
    std::set<entity::Geometry::Ptr> results;

    /*
        NOTE:   I'd prefer "NOT_WITHIN" etc instead of "NOTWITHIN", but sadly newer versions of
                libsqlite3-dev (3.11) have a
                    #define NOT_WITHIN 0
                inside of sqlite3.h, which makes the preprocessor expand it here, and the compiler
                throwing an error pointing at NOT_WITHIN...
    */
    // negative constraints have to be odd!
    enum QueryType {
        WITHIN      = 0, NOTWITHIN,
        CONTAINS    = 2, NOTCONTAINS,
        INTERSECTS  = 4, NOTINTERSECTS
    };  // ToDo rename it to SpatialQueryType and make a enum class out of it!

    /** gets the reference bounding box and geometry pair. It could either be 2D or 3D. The geometry will be null if its only a box based query. */
    ValuePair refBoxGeometryPair() { return refPair_; }

    /** returns the mode of the query: WITHIN, NOT_WITHIN, ... */
    QueryType mode() const { return qtype_; }

    /** set the mode of the query */
    void mode(QueryType m) { qtype_ = m; }

    /** inverts the mode: WITHIN <-> NOTWITHIN etc. */
    void invert()
    {
    /*  inversion of query type:
        positive (WITHIN, CONTAINS, INTERSECTS) are even (0, 2, 4),
        negative (NOT_WITHIN, ...) are odd (1, 3, 5).
        Just increment even and decrement odd values.    */
        qtype_ = (qtype_ % 2 == 0) ? QueryType(qtype_+1) : QueryType(qtype_-1);
    }

    /**
        Query for everything within the bounding box of 'geometry'. Passes the Envelope3D of
        the geometry to 'withinBox'.
    */
    static SpatialIndexQuery::Ptr withinBoxOf(entity::Geometry::Ptr geometry)
    {
        return SpatialIndexQuery::createBoxQuery(geometry, SpatialIndexQuery::WITHIN);
    }

    static SpatialIndexQuery::Ptr containsBoxOf(entity::Geometry::Ptr geometry)
    {
        return SpatialIndexQuery::createBoxQuery(geometry, SpatialIndexQuery::CONTAINS);
    }

    static SpatialIndexQuery::Ptr intersectsBoxOf(entity::Geometry::Ptr geometry)
    {
        return SpatialIndexQuery::createBoxQuery(geometry, SpatialIndexQuery::INTERSECTS);
    }


    /**
        Query for everything within the bbox specified. Explicit coordinate system.
        This creates a new GeometryCollection that is used for the query.
     */
    static SpatialIndexQuery::Ptr withinBox(const EigenVector& lower, const EigenVector& upper, entity::SpatialReference::Ptr cs)
    {
        return SpatialIndexQuery::createBoxQuery(lower, upper, cs, SpatialIndexQuery::WITHIN);
    }

    static SpatialIndexQuery::Ptr containsBox(  const EigenVector& lower, const EigenVector& upper, entity::SpatialReference::Ptr cs)
    {
        return SpatialIndexQuery::createBoxQuery(lower, upper, cs, SpatialIndexQuery::CONTAINS);
    }

    static SpatialIndexQuery::Ptr intersectsBox(const EigenVector& lower, const EigenVector& upper, entity::SpatialReference::Ptr cs)
    {
        return SpatialIndexQuery::createBoxQuery(lower, upper, cs, SpatialIndexQuery::INTERSECTS);
    }

    // TODO Disjoint and Overlaps
    // TODO Query for a specific geometry relations not only Boxes!
private:

    // the actual type
    QueryType qtype_;

    // the reference pair of bounding box and geometry
    ValuePair refPair_;

    // the reference coordinate system of this query. (same as geometry reference if geometry is not null)
    entity::SpatialReference::Ptr referenceCS_;

    // use static methods to construct queries
    SpatialIndexQuery() {}

    void setupPair(const EigenVector& lower, const EigenVector& upper, entity::Geometry::Ptr geo, entity::SpatialReference::Ptr cs)
    {
        ValuePair pair;
        pair.first = createBox(lower, upper);
        pair.second = geo;

        this->refPair_ = pair;
        this->referenceCS_ = cs;
    }

    Box createBox(const EigenVector& min, const EigenVector& max)
    {
        return Box(toPoint(min), toPoint(max));
    }

    // helper: create query from geo or upper/lower and a type
    static SpatialIndexQuery::Ptr createBoxQuery(entity::Geometry::Ptr geometry, QueryType type)
    {
        geos::geom::Coordinate min, max;
        geometry->findEnvelope(min, max);

        auto lower = toVector(min);
        auto upper = toVector(max);

        return createBoxQuery(lower, upper, geometry->getCS(), type);
    }

    static SpatialIndexQuery::Ptr createBoxQuery(const EigenVector& min, const EigenVector& max,
                                        entity::SpatialReference::Ptr cs, QueryType type)
    {
         if (!cs) return SpatialIndexQuery::Ptr();

        SpatialIndexQuery::Ptr query(new SpatialIndexQuery());
        query->setupPair(min, max, nullptr, cs);
        query->mode(type);
        return query;
    }

    static Point toPoint(const EigenVector& vec)
    {
        if (dim == 2)
        {
            return Point(vec.x(), vec.y());
        }
        else if (dim == 3)
        {
            return Point(vec.x(), vec.y(), vec.z());
        }
        return Point();
    }

    static EigenVector toVector(const geos::geom::Coordinate& coord)
    {
        EigenVector vec;

        if (dim >= 2)
        {
            vec.x() = coord.x;
            vec.y() = coord.y;
        }
        if (dim >= 3)
        {
            vec.z() = coord.z;
        }

        return vec;
    }

};


}}

#endif /* end of include guard SEMPR_QUERY_SPATIALQUERY_HPP_ */
