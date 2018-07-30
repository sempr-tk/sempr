#include <sempr/entity/spatial/PointCloud_2.hpp>
#include <PointCloud_2_odb.h>

#include <sempr/core/RDF.hpp>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(PointCloud2)

PointCloud2::PointCloud2() : PointCloud2(new sempr::core::IDGen<PointCloud2>())
{
}

PointCloud2::PointCloud2(const sempr::core::IDGenBase* idgen) : sempr::entity::MultiPoint(idgen)
{
    setDiscriminator<PointCloud2>();

}


/*
void PointCloud2::removePoint(uint64_t i)
{
    // TODO
    changed();
}
*/

void PointCloud2::setCoordinatesWithColor(const std::vector<geom::Coordinate> &coordinates,
                                    std::vector<unsigned char> const& colors)
{
    setCoordinates(coordinates);

    //Color is todo.
    changed();
}

}}
