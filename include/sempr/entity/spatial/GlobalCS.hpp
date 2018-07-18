#ifndef SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_
#define SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_

#include <sempr/entity/spatial/SpatialReference.hpp>
#include <geos/geom/CoordinateFilter.h>
//#include <ogr_spatialref.h>

namespace sempr { namespace entity {

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

    /**
        Create an OGRCoordinateTransformation from this to other. Only available for global
        coordinate systems that use a OGRSpatialReference, as this task is simply delegated to GDAL.
    */
    //std::shared_ptr<OGRCoordinateTransformation> to(GlobalCS::Ptr other);

    //from this to other
    std::shared_ptr<geos::geom::CoordinateFilter> to(const GlobalCS::Ptr other);


protected:
    GlobalCS();
    GlobalCS(const core::IDGenBase*);

    /// used by both projection and geographic coordinate systems.
    //#pragma db type("TEXT")
    //OGRSpatialReference frame_;

private:
    friend class odb::access;

};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_ */
