#include <sempr/entity/spatial/PointCloud.hpp>
#include <PointCloud_odb.h>

#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(PointCloud)

PointCloud::PointCloud() : PointCloud(new sempr::core::IDGen<PointCloud>())
{
}

PointCloud::PointCloud(const sempr::core::IDGenBase* idgen) : sempr::entity::MultiPoint(idgen)
{
    setDiscriminator<PointCloud>();

}


/*
void PointCloud::removePoint(uint64_t i)
{
    // TODO
    changed();
}
*/

void PointCloud::setCoordinatesWithColor(const std::vector<geom::Coordinate> &coordinates,
                                    std::vector<unsigned char> const& colors)
{
    setCoordinates(coordinates);

    //Color is todo.
    changed();
}

}}
