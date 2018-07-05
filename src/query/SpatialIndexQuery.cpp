#include <sempr/query/SpatialIndexQuery.hpp>
#include <sempr/entity/spatial/GeometryCollection.hpp>

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
    entity::GeometryCollection::Ptr corners(new entity::GeometryCollection());
    /*
    corners->setCS(cs);

    OGRPoint* p;
    p = new OGRPoint(lower.x(), lower.y(), lower.z()); corners->geometry()->addGeometryDirectly(p);
    p = new OGRPoint(lower.x(), lower.y(), upper.z()); corners->geometry()->addGeometryDirectly(p);
    p = new OGRPoint(lower.x(), upper.y(), lower.z()); corners->geometry()->addGeometryDirectly(p);
    p = new OGRPoint(lower.x(), upper.y(), upper.z()); corners->geometry()->addGeometryDirectly(p);
    p = new OGRPoint(upper.x(), lower.y(), lower.z()); corners->geometry()->addGeometryDirectly(p);
    p = new OGRPoint(upper.x(), lower.y(), upper.z()); corners->geometry()->addGeometryDirectly(p);
    p = new OGRPoint(upper.x(), upper.y(), lower.z()); corners->geometry()->addGeometryDirectly(p);
    p = new OGRPoint(upper.x(), upper.y(), upper.z()); corners->geometry()->addGeometryDirectly(p);
    this->refGeo_ = corners;
     */
}

SpatialIndexQuery::Ptr SpatialIndexQuery::createQuery(
    entity::Geometry::Ptr geometry, sempr::query::SpatialIndexQuery::QueryType type)
{
    /* todo
    OGREnvelope3D env;
    geometry->geometry()->getEnvelope(&env);
    */

    Eigen::Vector3d lower, upper;
    /* todo
    lower.x() = env.MinX;
    lower.y() = env.MinY;
    lower.z() = env.MinZ;
    upper.x() = env.MaxX;
    upper.y() = env.MaxY;
    upper.z() = env.MaxZ;
     */

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
