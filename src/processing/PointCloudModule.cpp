#include <sempr/processing/PointCloudModule.hpp>

namespace sempr { namespace processing {

PointCloudModule::PointCloudModule()
{
    // nothing to do?
}

void PointCloudModule::process(core::EntityEvent<entity::PointCloud>::Ptr cloudEvent)
{
    //TODO
}

void PointCloudModule::process(core::EntityEvent<entity::agro::AgroEntity>::Ptr agroEvent)
{

}

}}
