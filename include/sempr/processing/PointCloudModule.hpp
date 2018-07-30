#ifndef POINTCLOUDMODULE_HPP
#define POINTCLOUDMODULE_HPP

#include <sempr/processing/Module.hpp>
//#include <sempr/entity/PointCloud.hpp>
#include <sempr/entity/spatial/PointCloud_2.hpp>
#include <sempr/entity/agro/AgroEntity.hpp>
#include <sempr/query/ObjectQuery.hpp>
#include <sempr/query/PolygonQuery.hpp>

//#include <PointCloud_odb.h>
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

    //void process(core::EntityEvent<entity::PointCloud>::Ptr cloudEvent) override;

    /* Get the internal Points of an Polygon. Points can be from MultiPoint or PointCloud */
    void process(query::PolygonQuery::Ptr query) override;
};

}}

#endif // POINTCLOUDMODULE_HPP
