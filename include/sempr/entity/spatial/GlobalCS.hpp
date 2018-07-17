#ifndef SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_
#define SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_

#include <sempr/entity/spatial/SpatialReference.hpp>
#include <geos/geom/CoordinateFilter.h>
//#include <ogr_spatialref.h>

namespace sempr { namespace entity {

/// List of supported reference earth ellipsoid models (see: https://en.wikipedia.org/wiki/Earth_ellipsoid)
enum struct GeodeticReference 
{
    WGS84,      // Globale GPS Reference, base of MGRS/UTM/UPS
    EPSG4326,   // same as WGS84
    GRS80,      // Global ITRS
    NAD83,      // same as GRS 80 Ellipsoid
    NAD27,      // same as Clarke 1866 Ellipsoid
    IERS03,
    HAYFORD,    // same as Internation 1924
    ED50        // same as Hayford, base of Gaus-Krüger and early UTM (up to 1980s)
};

/**
    Simple base class for ProjectionCS and GeographicCS, as both are root, and both need to
    be able to create an OGRCoordinateTransformation to the other. It provides implementations of
    transformation[To|From]Root (which only return identity-matrices), getRoot (which returns this),
    and a method to compute the transformation from this to another global coordinate system.
*/
#pragma db object
class GlobalCS : public SpatialReference {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<GlobalCS>;

    SpatialReference::Ptr getRoot() override;
    Eigen::Affine3d transformationToRoot() const override;
    Eigen::Affine3d transformationFromRoot() const override;
    virtual ~GlobalCS();

    double a();
    double f();

    /**
     * @brief Check if this global coordinate system and an other have the same geodetic reference (e.g. WG84 or GRS80 ..).
     * 
     * @param other GlobalCS to be checked
     * @return true if this and the other coordinate system have the the geodetic reference
     */
    bool sameReference(const GlobalCS::Ptr other) const;

    /**
        Create an OGRCoordinateTransformation from this to other. Only available for global
        coordinate systems that use a OGRSpatialReference, as this task is simply delegated to GDAL.
    */
    //std::shared_ptr<OGRCoordinateTransformation> to(GlobalCS::Ptr other);

    //from this to other
    std::shared_ptr<geos::geom::CoordinateFilter> to(const GlobalCS::Ptr other);


protected:
    GlobalCS();
    GlobalCS(const core::IDGenBase*, GeodeticReference reference = GeodeticReference::WGS84);

    GeodeticReference reference_;

    double a_;   //semi-major axis
    double f_;   //inverse flattering

    /// used by both projection and geographic coordinate systems.
    //#pragma db type("TEXT")
    //OGRSpatialReference frame_;

private:
    friend class odb::access;

    void initParameter(GeodeticReference reference);
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_ */
