#include <sempr/entity/spatial/PointCloud.hpp>
#include <PointCloud_odb.h>

#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(PointCloud)

PointCloud::PointCloud() : 
    PointCloud(new sempr::core::IDGen<PointCloud>())
{
}

PointCloud::PointCloud(const sempr::core::IDGenBase* idgen) : 
    sempr::entity::MultiPoint(idgen)
{
    setDiscriminator<PointCloud>();
}

PointCloud::~PointCloud()
{

}


}}
