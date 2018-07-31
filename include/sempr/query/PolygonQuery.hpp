#ifndef SEMPR_QUERY_POLYGONQUERY_HPP_
#define SEMPR_QUERY_POLYGONQUERY_HPP_

#include <geos/geom/Polygon.h>
#include <geos/geom/Coordinate.h>
#include <odb/core.hxx>

#include <sempr/query/Query.hpp>
#include <sempr/storage/DBObject.hpp>
#include <AgroEntity_odb.h>
#include <Polygon_odb.h>

namespace sempr { namespace query {

    class PolygonQuery : public Query, public core::OType<PolygonQuery> {
    public:
        using Ptr = std::shared_ptr<PolygonQuery>;

        PolygonQuery(const geom::Polygon *poly);
        PolygonQuery(entity::agro::AgroEntity::Ptr &agro);
        PolygonQuery(entity::Polygon::Ptr &polygon);

        ~PolygonQuery();

        std::string type() const override { return "PolygonQuery"; }

        /** the vector of points, that are within the polygon of this entity **/
        std::vector <geom::Coordinate> results;     

        const geom::Polygon* geometry() const { return m_poly; }

    private:

        PolygonQuery();

        const geom::Polygon *m_poly;

    };
}}

#endif /* end of include guard: SEMPR_QUERY_POLYGONQUERY_HPP_ */
