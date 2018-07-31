#include <sempr/query/PolygonQuery.hpp>
#include <sempr/entity/spatial/GeometryCollection.hpp>

namespace sempr { namespace query {

PolygonQuery::PolygonQuery()
{
}

PolygonQuery::PolygonQuery(const geom::Polygon *poly) : m_poly(poly)
{
    results.reserve(1); //what is a useful size here?!
}

// this is really confusing with all the getGeometry, setGeometry, geometry() functions -
// with GDAL the only implementation of geometry() for everything was so beautiful :(
PolygonQuery::PolygonQuery(entity::agro::AgroEntity::Ptr &agro) : PolygonQuery(agro->geometry())
{
}

PolygonQuery::PolygonQuery(entity::Polygon::Ptr &polygon) : PolygonQuery(polygon->getGeometry())
{
}

PolygonQuery::~PolygonQuery()
{
}

}}
