#include <sempr/query/SpatialIndexQuery.hpp>
#include <sempr/entity/spatial/GeometryCollection.hpp>

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

SpatialIndexQuery::Ptr SpatialIndexQuery::withinBox( const Eigen::Vector3d& lower,
                                        const Eigen::Vector3d& upper,
                                        entity::SpatialReference::Ptr cs)
{
    if (!cs) return SpatialIndexQuery::Ptr();

    /* -- this could/should be a helper function -- */
    entity::GeometryCollection::Ptr corners(new entity::GeometryCollection());
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
    /* -- ---- -- */

    SpatialIndexQuery::Ptr query(new SpatialIndexQuery());
    query->mode(SpatialIndexQuery::WITHIN);
    query->refGeo_ = corners;

    return query;
}


SpatialIndexQuery::Ptr SpatialIndexQuery::withinBoxOf(entity::Geometry::Ptr geometry)
{
    OGREnvelope3D env;
    geometry->geometry()->getEnvelope(&env);
    Eigen::Vector3d lower, upper;
    lower.x() = env.MinX;
    lower.y() = env.MinY;
    lower.z() = env.MinZ;
    upper.x() = env.MaxX;
    upper.y() = env.MaxY;
    upper.z() = env.MaxZ;

    return SpatialIndexQuery::withinBox(lower, upper, geometry->getCS());
}

}}
