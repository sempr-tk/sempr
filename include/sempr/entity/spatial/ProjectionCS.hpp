#ifndef SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_
#define SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_

#include <sempr/entity/spatial/SpatialReference.hpp>
#include <ogr_spatialref.h>

namespace sempr { namespace entity {


/**
    A projected coordinate system, using a base-geographic system (e.g. WGS84) and a
    projection into cartesian space.
    It is also a root of transformations.
*/
#pragma db object
class ProjectionCS : public SpatialReference {
public:
    using Ptr = std::shared_ptr<ProjectionCS>;
    SpatialReference::Ptr getRoot() override;
    Eigen::Affine3d transformationToRoot() const override;
    Eigen::Affine3d transformationFromRoot() const override;

    /**
        Create a ProjectionCS for a given UTM zone and a base geographic system.
        \param zone UTM zone
        \param north true if on northern hemisphere
        \param base geographic system to use, e.g. WGS84
    */
    static ProjectionCS::Ptr CreateUTM(int zone, bool north = true, const std::string& base = "WGS84");

    /**
        Create a ProjectionCS by applying an equirectangular projection centered at the given
        lat/lon coordinates in the chosen base system.
    */
    static ProjectionCS::Ptr CreateEquirect(double lat, double lon, const std::string& base = "WGS84");

private:
    ProjectionCS(); // just for ODB.
    friend class odb::access;
    #pragma db type("TEXT")
    OGRSpatialReference frame_;
};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_ */
