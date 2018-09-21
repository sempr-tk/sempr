#ifndef SEMPR_PROCESSING_SPATIALINDEXBASE_HPP_
#define SEMPR_PROCESSING_SPATIALINDEXBASE_HPP_

#include <sempr/entity/spatial/Geometry.hpp>

#include <geos/geom/Coordinate.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <Geometry_odb.h>   // required for EntityEvent<Geometry>
#include <SpatialReference_odb.h>   // required for EntityEvent<SpatialReference>


namespace sempr { namespace processing {

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

/*
    NOTE:   I'd prefer "NOT_WITHIN" etc instead of "NOTWITHIN", but sadly newer versions of
            libsqlite3-dev (3.11) have a
                #define NOT_WITHIN 0
            inside of sqlite3.h, which makes the preprocessor expand it here, and the compiler
            throwing an error pointing at NOT_WITHIN...
*/
// negative constraints have to be odd!
enum class SpatialQueryType {
    WITHIN      = 0, NOTWITHIN,
    CONTAINS    = 2, NOTCONTAINS,
    INTERSECTS  = 4, NOTINTERSECTS
};

template<std::size_t dim>
struct SpatialIndexBase
{
    /**
    Specify what is stored in the R-Tree:
        boxes, made out of points, consisting of 3 double, in cartesian space.
    NOTE: Boost seems to support geographic and spherical coordinates (lat-long etc) here, how
    does this affect the RTree? Can we use this to support indexing on lat-lon later on?
    */
};


template<>
struct SpatialIndexBase<2>
{
    typedef bg::model::point<double, 2, bg::cs::cartesian> Point;
    typedef bg::model::box<Point> Box;
    typedef std::pair<Box, entity::Geometry::Ptr> ValuePair;  // pair af a bounding box and a translated geometry clone
    typedef bgi::rtree<ValuePair, bgi::quadratic<16> > RTree;

    typedef Eigen::Matrix<double, 2, 1> Vector;


    static geos::geom::Coordinate toCoordinate(const Point& point)
    {
        return geos::geom::Coordinate(bg::get<0>(point), bg::get<1>(point));
    }

    static Point toPoint(const geos::geom::Coordinate& coord)
    {
        return Point(coord.x, coord.y);
    }

    static Point toPoint(const Vector& vec)
    {
        return Point(vec[0], vec[1]);
    }

    static Vector toEigen(const geos::geom::Coordinate& coord)
    {
        return Vector(coord.x, coord.y);
    }

    static std::vector<geos::geom::Coordinate> buildCorners(const geos::geom::Coordinate& min, const geos::geom::Coordinate& max)
    {
        std::vector<geos::geom::Coordinate> cornerCoordinates;

        geos::geom::Coordinate coord;
        coord = geos::geom::Coordinate(min.x, min.y); cornerCoordinates.push_back(coord);
        coord = geos::geom::Coordinate(min.x, max.y); cornerCoordinates.push_back(coord);
        coord = geos::geom::Coordinate(max.x, min.y); cornerCoordinates.push_back(coord);
        coord = geos::geom::Coordinate(max.x, max.y); cornerCoordinates.push_back(coord);

        return cornerCoordinates;
    }
};

template<>
struct SpatialIndexBase<3>
{
    typedef bg::model::point<double, 3, bg::cs::cartesian> Point;
    typedef bg::model::box<Point> Box;
    typedef std::pair<Box, entity::Geometry::Ptr> ValuePair;  // pair af a bounding box and a translated geometry clone
    typedef bgi::rtree<ValuePair, bgi::quadratic<16> > RTree;

    typedef Eigen::Matrix<double, 3, 1> Vector;


    static geos::geom::Coordinate toCoordinate(const Point& point)
    {
        return geos::geom::Coordinate(bg::get<0>(point), bg::get<1>(point), bg::get<2>(point));
    }

    static Point toPoint(const geos::geom::Coordinate& coord)
    {
        return Point(coord.x, coord.y, coord.z);
    }

    static Point toPoint(const Vector& vec)
    {
        return Point(vec[0], vec[1], vec[2]);
    }

    static Vector toEigen(const geos::geom::Coordinate& coord)
    {
        return Vector(coord.x, coord.y, coord.z);
    }

    static std::vector<geos::geom::Coordinate> buildCorners(const geos::geom::Coordinate& min, const geos::geom::Coordinate& max)
    {
        std::vector<geos::geom::Coordinate> cornerCoordinates;

        geos::geom::Coordinate coord;
        coord = geos::geom::Coordinate(min.x, min.y, min.z); cornerCoordinates.push_back(coord);
        coord = geos::geom::Coordinate(min.x, min.y, max.z); cornerCoordinates.push_back(coord);
        coord = geos::geom::Coordinate(min.x, max.y, min.z); cornerCoordinates.push_back(coord);
        coord = geos::geom::Coordinate(min.x, max.y, max.z); cornerCoordinates.push_back(coord);
        coord = geos::geom::Coordinate(max.x, min.y, min.z); cornerCoordinates.push_back(coord);
        coord = geos::geom::Coordinate(max.x, min.y, max.z); cornerCoordinates.push_back(coord);
        coord = geos::geom::Coordinate(max.x, max.y, min.z); cornerCoordinates.push_back(coord);
        coord = geos::geom::Coordinate(max.x, max.y, max.z); cornerCoordinates.push_back(coord);

        return cornerCoordinates;
    }
};


}}

#endif /* end of include guard SEMPR_PROCESSING_SPATIALINDEX_HPP_ */
