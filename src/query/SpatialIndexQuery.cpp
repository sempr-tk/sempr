#include <sempr/query/SpatialIndexQuery.hpp>
#include <sempr/entity/spatial/MultiPoint.hpp>

#include <geos/geom/Envelope.h>

namespace sempr { namespace query {

SpatialIndexQuery::SpatialIndexQuery()
{
}

SpatialIndexQuery::~SpatialIndexQuery()
{
}

SpatialIndexQuery::QueryType SpatialIndexQuery::mode() const
{
    return qtype_;
}

void SpatialIndexQuery::mode(SpatialIndexQuery::QueryType m)
{
    qtype_ = m;
}

entity::Geometry::Ptr SpatialIndexQuery::refGeo()
{
    return refGeo_;
}


void SpatialIndexQuery::setupRefGeo(const Eigen::Vector3d &lower, const Eigen::Vector3d &upper,
                                    entity::SpatialReference::Ptr cs)
{
    entity::MultiPoint::Ptr corners(new entity::MultiPoint());

    corners->setCS(cs);

    geos::geom::Coordinate coord;
    std::vector<geos::geom::Coordinate> cornerCoordinates;
    coord = geos::geom::Coordinate(lower.x(), lower.y(), lower.z()); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(lower.x(), lower.y(), upper.z()); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(lower.x(), upper.y(), lower.z()); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(lower.x(), upper.y(), upper.z()); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(upper.x(), lower.y(), lower.z()); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(upper.x(), lower.y(), upper.z()); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(upper.x(), upper.y(), lower.z()); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(upper.x(), upper.y(), upper.z()); cornerCoordinates.push_back(coord);

    corners->setCoordinates(cornerCoordinates);

    this->refGeo_ = corners;
}

SpatialIndexQuery::Ptr SpatialIndexQuery::createQuery(
    entity::Geometry::Ptr geometry, sempr::query::SpatialIndexQuery::QueryType type)
{
    geos::geom::Coordinate min, max;
    geometry->findEnvelope(min, max);

    Eigen::Vector3d lower, upper;

    lower.x() = min.x;
    lower.y() = min.y;
    lower.z() = min.z;
    upper.x() = max.x;
    upper.y() = max.y;
    upper.z() = max.z;

    return createQuery(lower, upper, geometry->getCS(), type);
}

SpatialIndexQuery::Ptr SpatialIndexQuery::createQuery(
    const Eigen::Vector3d &lower, const Eigen::Vector3d &upper,
    entity::SpatialReference::Ptr cs, sempr::query::SpatialIndexQuery::QueryType type)
{
    if (!cs) return SpatialIndexQuery::Ptr();

    SpatialIndexQuery::Ptr query(new SpatialIndexQuery());
    query->setupRefGeo(lower, upper, cs);
    query->mode(type);
    return query;
}


void SpatialIndexQuery::invert()
{
    /*
        inversion of query type:
        positive (WITHIN, CONTAINS, INTERSECTS) are even (0, 2, 4),
        negative (NOT_WITHIN, ...) are odd (1, 3, 5).
        Just increment even and decrement odd values.
    */
    if (qtype_ % 2 == 0) qtype_ = QueryType(qtype_+1);
    else qtype_ = QueryType(qtype_-1);
}


SpatialIndexQuery::Ptr SpatialIndexQuery::withinBox( const Eigen::Vector3d& lower,
                                        const Eigen::Vector3d& upper,
                                        entity::SpatialReference::Ptr cs)
{
    return SpatialIndexQuery::createQuery(lower, upper, cs, SpatialIndexQuery::WITHIN);
}

SpatialIndexQuery::Ptr SpatialIndexQuery::containsBox( const Eigen::Vector3d& lower,
                                        const Eigen::Vector3d& upper,
                                        entity::SpatialReference::Ptr cs)
{
    return SpatialIndexQuery::createQuery(lower, upper, cs, SpatialIndexQuery::CONTAINS);
}

SpatialIndexQuery::Ptr SpatialIndexQuery::intersectsBox( const Eigen::Vector3d& lower,
                                        const Eigen::Vector3d& upper,
                                        entity::SpatialReference::Ptr cs)
{
    return SpatialIndexQuery::createQuery(lower, upper, cs, SpatialIndexQuery::INTERSECTS);
}


SpatialIndexQuery::Ptr SpatialIndexQuery::withinBoxOf(entity::Geometry::Ptr geometry)
{
    return SpatialIndexQuery::createQuery(geometry, SpatialIndexQuery::WITHIN);
}

SpatialIndexQuery::Ptr SpatialIndexQuery::containsBoxOf(entity::Geometry::Ptr geometry)
{
    return SpatialIndexQuery::createQuery(geometry, SpatialIndexQuery::CONTAINS);
}

SpatialIndexQuery::Ptr SpatialIndexQuery::intersectsBoxOf(entity::Geometry::Ptr geometry)
{
    return SpatialIndexQuery::createQuery(geometry, SpatialIndexQuery::INTERSECTS);
}

}}
