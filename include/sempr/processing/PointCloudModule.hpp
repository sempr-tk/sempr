#ifndef POINTCLOUDMODULE_HPP
#define POINTCLOUDMODULE_HPP

#include <geos/geom/Polygon.h>
#include <geos/geom/Coordinate.h>

#include <sempr/processing/Module.hpp>
#include <sempr/query/ObjectQuery.hpp>
#include <sempr/query/PolygonQuery.hpp>
#include <sempr/query/SpatialIndexQuery.hpp>
#include <PointCloud_odb.h>
#include <AgriEntity_odb.h>

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

    void calculatePoints(const entity::PointCloud::Ptr cloud, query::PolygonQuery::Ptr query);
    
    void calculatePoints(const entity::PointCloud::Ptr cloud, std::vector <geos::geom::Coordinate> coords);
    
    bool intersect(std::vector<geom::Coordinate> c, std::vector <geom::Coordinate> p);

    std::vector<geom::Coordinate> m_c;
    std::vector<double> m_r;
    std::vector<double> m_g;
    std::vector<double> m_b;

    bool m_colors = true;
};

}}

#endif // POINTCLOUDMODULE_HPP
