#ifndef SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_
#define SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_

#include <sempr/entity/spatial/GlobalCS.hpp>
#include <ogr_spatialref.h>

namespace sempr { namespace entity {

// forward declaration of GeographicCS -- both need to know each other
class GeographicCS;


/**
    A projected coordinate system, using a base-geographic system (e.g. WGS84) and a
    projection into cartesian space. It is also a root of transformations (does not have a parent).
*/
#pragma db object
class ProjectionCS : public GlobalCS {
public:
    using Ptr = std::shared_ptr<ProjectionCS>;
    ENTITY_DEFAULT_EVENT_METHODS(ProjectionCS, GlobalCS);

    SpatialReference::Ptr getRoot() override;
    Eigen::Affine3d transformationToRoot() const override;
    Eigen::Affine3d transformationFromRoot() const override;

    /**
        Create a ProjectionCS for a given UTM zone and a base geographic system.
        NOTE: This is not the UTM you know. This is bare UTM, i.e. the world sliced in 6° longitude and split in north / south. What you might know as e.g. UTM zone 32U is an extension to that, called MGRS (military grade reference system), sometimes known as UTMREF, which introduces finer grained zones. This is not supported right now -- BUT: GDAL has an undocumented feature for MGRS, take a look at frmts/nitf/mgrs.c, maybe you can use that in some way.
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
};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_PROJECTIONCS_HPP_ */
