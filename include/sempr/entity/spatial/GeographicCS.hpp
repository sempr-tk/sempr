#ifndef SEMPR_ENTITY_SPATIAL_GEOGRAPHICCS_HPP_
#define SEMPR_ENTITY_SPATIAL_GEOGRAPHICCS_HPP_

#include <sempr/entity/spatial/SpatialReference.hpp>
#include <ogr_spatialref.h>

namespace sempr { namespace entity {


/**
    A geographic reference system, e.g. WGS84.
    It is also a root of transformations.
*/
#pragma db object
class GeographicCS : public SpatialReference {
public:
    using Ptr = std::shared_ptr<GeographicCS>;
    SpatialReference::Ptr getRoot() override;
    Eigen::Affine3d transformationToRoot() const override;
    Eigen::Affine3d transformationFromRoot() const override;

    /**
        Create a geographic coordinate system based on a well known text,
        e.g. "WGS84", "WGS72", "NAD27", "NAD83" or any "EPSG:n"
    */
    GeographicCS(const std::string& name);
    GeographicCS(const std::string& name, const core::IDGenBase*);

private:
    GeographicCS(); // just for ODB.
    friend class odb::access;
    #pragma db type("TEXT")
    OGRSpatialReference frame_;
};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEOGRAPHICCS_HPP_ */
