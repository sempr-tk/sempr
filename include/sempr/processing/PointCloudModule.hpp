#ifndef POINTCLOUDMODULE_HPP
#define POINTCLOUDMODULE_HPP

#include <sempr/processing/Module.hpp>
#include <sempr/entity/PointCloud.hpp>
#include <sempr/entity/agro/AgroEntity.hpp>

#include <PointCloud_odb.h>
#include <AgroEntity_odb.h>

namespace sempr { namespace processing {

class PointCloudModule : public Module< core::EntityEvent<entity::PointCloud>, core::EntityEvent<entity::agro::AgroEntity> >
{
public:
    PointCloudModule();

void process(core::EntityEvent<entity::PointCloud>::Ptr cloudEvent) override;

void process(core::EntityEvent<entity::agro::AgroEntity>::Ptr agroEvent) override;
};

}}

#endif // POINTCLOUDMODULE_HPP
