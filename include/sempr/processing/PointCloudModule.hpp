#ifndef POINTCLOUDMODULE_HPP
#define POINTCLOUDMODULE_HPP

#include <geos/geom/Polygon.h>
#include <geos/geom/Coordinate.h>

#include <sempr/processing/Module.hpp>
#include <sempr/query/ObjectQuery.hpp>
#include <sempr/query/PolygonQuery.hpp>
#include <PointCloud_2_odb.h>
#include <AgroEntity_odb.h>

namespace sempr { namespace processing {

class PointCloudModule : public Module< query::PolygonQuery >
{
public:
    using Ptr = std::shared_ptr<PointCloudModule>;

    PointCloudModule();

    ~PointCloudModule();

    virtual std::string type() const override;

    /* Get the internal Points of an Polygon. Points can be from MultiPoint or PointCloud */
    void process(query::PolygonQuery::Ptr query) override;

private:
    int checkIntersection(geom::Coordinate v11, geom::Coordinate v12, geom::Coordinate v21, geom::Coordinate v22);

    void calculatePoints(const entity::PointCloud2::Ptr cloud, query::PolygonQuery::Ptr query);
};

}}

#endif // POINTCLOUDMODULE_HPP
