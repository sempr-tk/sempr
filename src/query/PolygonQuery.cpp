#include <sempr/query/PolygonQuery.hpp>
#include <sempr/entity/spatial/GeometryCollection.hpp>

namespace sempr { namespace query {

PolygonQuery::PolygonQuery()
{
}

PolygonQuery::PolygonQuery(const entity::Polygon::Ptr &poly, const double low, const double high) :
    m_poly(poly),
    m_low(low),
    m_high(high)
{
}

// this is really confusing with all the getGeometry, setGeometry, geometry() functions -
// with GDAL the only implementation of geometry() for everything was so beautiful :(
PolygonQuery::PolygonQuery(entity::agri::AgriEntity::Ptr &agri) :
    PolygonQuery(agri->geometry(), agri->low(), agri->high())
{
}


PolygonQuery::~PolygonQuery()
{
}

}}
