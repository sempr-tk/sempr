#include <sempr/processing/PointCloudModule.hpp>

namespace sempr { namespace processing {

PointCloudModule::PointCloudModule()
{
    // nothing to do?
    std::cout << "created PCM" << std::endl;
}

/*
void PointCloudModule::process(core::EntityEvent<entity::PointCloud>::Ptr cloudEvent)
{
    // TODO
}
*/

PointCloudModule::~PointCloudModule()
{
    // nothing to do?
}

std::string PointCloudModule::type() const {
    return "PointCloudModule";
}

void PointCloudModule::process(query::PolygonQuery::Ptr query)
{
    //TODO
    query::ObjectQuery<entity::PointCloud2>::Ptr clouds(new query::ObjectQuery<entity::PointCloud2>() );

    ask(clouds);

    std::cout << "asked" << std::endl;

    for (auto const& cloud : clouds->results)
    {
        std::cout << "works" << std::endl;
        query->calculatePoints(cloud);
    }
}

}}
