#ifndef SEMPR_QUERY_CALCULATEPOINTSQUERY_HPP_
#define SEMPR_QUERY_CALCULATEPOINTSQUERY_HPP_

#include <geos/geom/Polygon.h>
#include <geos/geom/Coordinate.h>
#include <odb/core.hxx>

#include <sempr/query/Query.hpp>
#include <sempr/storage/DBObject.hpp>
#include <AgriEntity_odb.h>
#include <Polygon_odb.h>
#include <PointCloud_odb.h>

namespace sempr { namespace query {

    class CalculatePointsQuery : public Query, public core::OType<CalculatePointsQuery> {
    public:
        using Ptr = std::shared_ptr<CalculatePointsQuery>;

        CalculatePointsQuery(const entity::Polygon::Ptr &polygon, const double min_heigh, const double max_height);
        CalculatePointsQuery(entity::agri::AgriEntity::Ptr &agri);

        ~CalculatePointsQuery();

        std::string type() const override { return "CalculatePointsQuery"; }

        /** the vector of points, that are within the polygon of this entity **/
        entity::PointCloud::Ptr results;

        const entity::Polygon::Ptr entity() const { return m_poly; }

        const double minHeight() { return _min_height; }
        const double maxHeight() { return _max_height; }

    private:

        CalculatePointsQuery();

        const entity::Polygon::Ptr m_poly;
        double _min_height;
        double _max_height;

    };
}}

#endif /* end of include guard: SEMPR_QUERY_CALCULATEPOINTSQUERY_HPP_ */
