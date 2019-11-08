#ifndef SEMPR_PROCESSING_CALCULATEPOINTSMODULE_HPP
#define SEMPR_PROCESSING_CALCULATEPOINTSMODULE_HPP

#include <geos/geom/Polygon.h>
#include <geos/geom/Coordinate.h>

#include <sempr/processing/Module.hpp>
#include <sempr/query/ObjectQuery.hpp>
#include <sempr/query/CalculatePointsQuery.hpp>
#include <sempr/query/SpatialIndexQuery.hpp>
#include <PointCloud_odb.h>
#include <AgriEntity_odb.h>

namespace sempr { namespace processing {

class CalculatePointsModule : public Module< query::CalculatePointsQuery >
{
public:
    using Ptr = std::shared_ptr<CalculatePointsModule>;

    CalculatePointsModule();

    ~CalculatePointsModule();

    virtual std::string type() const override;

    /* Get the internal Points of an Polygon. Points can be from MultiPoint or PointCloud */
    void process(query::CalculatePointsQuery::Ptr query) override;
    

private:
    int checkIntersection(geom::Coordinate v11, geom::Coordinate v12, geom::Coordinate v21, geom::Coordinate v22);

    void calculatePoints(const entity::PointCloud::Ptr cloud, query::CalculatePointsQuery::Ptr query, std::vector<geos::geom::Coordinate> &p, std::vector<uint8_t> &r, std::vector<uint8_t> &g, std::vector<uint8_t> &b, bool &color);
    
    void calculatePoints(const entity::PointCloud::Ptr cloud, std::vector <geos::geom::Coordinate> coords);
    
    bool intersect(std::vector<geom::Coordinate> c, std::vector <geom::Coordinate> p);



};







}}

#endif // SEMPR_PROCESSING_CALCULATEPOINTSMODULE_HPP
