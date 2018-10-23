#ifndef SEMPR_QUERY_POLYGONQUERY_HPP_
#define SEMPR_QUERY_POLYGONQUERY_HPP_

#include <geos/geom/Polygon.h>
#include <geos/geom/Coordinate.h>
#include <odb/core.hxx>

#include <sempr/query/Query.hpp>
#include <sempr/storage/DBObject.hpp>
#include <AgriEntity_odb.h>
#include <Polygon_odb.h>
#include <PointCloud_odb.h>

namespace sempr { namespace query {

    class PolygonQuery : public Query, public core::OType<PolygonQuery> {
    public:
        using Ptr = std::shared_ptr<PolygonQuery>;

        PolygonQuery(const geom::Polygon *poly, const double low, const double high);
        PolygonQuery(entity::agri::AgriEntity::Ptr &agri);
        PolygonQuery(entity::Polygon::Ptr &polygon, const double low, const double high);

        ~PolygonQuery();

        std::string type() const override { return "PolygonQuery"; }

        /** the vector of points, that are within the polygon of this entity **/
        entity::PointCloud::Ptr results;

        const geom::Polygon* geometry() const { return m_poly; }

        const double low() const { return m_low; }
        const double high() const { return m_high; }

    private:

        PolygonQuery();

        const geom::Polygon *m_poly;
        double m_low;
        double m_high;

    };
}}

#endif /* end of include guard: SEMPR_QUERY_POLYGONQUERY_HPP_ */
