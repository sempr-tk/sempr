#include <sempr/query/CalculatePointsQuery.hpp>
#include <sempr/entity/spatial/GeometryCollection.hpp>

namespace sempr { namespace query {

CalculatePointsQuery::CalculatePointsQuery()
{
}

CalculatePointsQuery::CalculatePointsQuery(const entity::Polygon::Ptr &poly, const double min_height, const double max_height) :
    m_poly(poly),
    _min_height(min_height),
    _max_height(max_height)
{
}

// this is really confusing with all the getGeometry, setGeometry, geometry() functions -
// with GDAL the only implementation of geometry() for everything was so beautiful :(
CalculatePointsQuery::CalculatePointsQuery(entity::agri::AgriEntity::Ptr &agri) :
    CalculatePointsQuery(agri->geometry(), agri->minHeight(), agri->maxHeight())
{
}


CalculatePointsQuery::~CalculatePointsQuery()
{
}

}}
