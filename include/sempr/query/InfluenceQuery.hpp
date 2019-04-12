#ifndef INFLUENCEQUERY_HPP
#define INFLUENCEQUERY_HPP

#include <odb/core.hxx>

#include <sempr/query/Query.hpp>
#include <sempr/storage/DBObject.hpp>

#include <AgriEntity_odb.h>
#include <PointCloud_odb.h>

namespace sempr { namespace query {

class InfluenceQuery : public Query, public core::OType<InfluenceQuery>
{
public:
    using Ptr = std::shared_ptr<InfluenceQuery>;

    InfluenceQuery(entity::agri::AgriEntity::Ptr &water, entity::agri::AgriEntity::Ptr &agri);

    ~InfluenceQuery();

    /** the vector of points, that are within the polygon of this entity **/
    entity::PointCloud::Ptr results;

    std::string type() const override { return "InfluenceQuery"; }
private:
    InfluenceQuery();
};

}}

#endif // INFLUENCEQUERY_HPP
