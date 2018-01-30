#ifndef SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_
#define SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_

#include <sempr/entity/spatial/SpatialReference.hpp>
#include <ogr_spatialref.h>

namespace sempr { namespace entity {


/**
    Simple base class for ProjectionCS and GeographicCS, as both are root, and both need to
    be able to create an OGRCoordinateTransformation to the other. This is mainly a stub to be
    filled by the derived classes, but provides the OGRSpatialReference and a method to create
    a transformation between two global coordinate systems.
*/
#pragma db object
class GlobalCS : public SpatialReference {
public:
    using Ptr = std::shared_ptr<GlobalCS>;
    ENTITY_DEFAULT_EVENT_METHODS(GlobalCS, SpatialReference);

    virtual SpatialReference::Ptr getRoot() override;
    Eigen::Affine3d transformationToRoot() const override;
    Eigen::Affine3d transformationFromRoot() const override;
    virtual ~GlobalCS();

    /**
        Create an OGRCoordinateTransformation from this to other. Only available for global
        coordinate systems that use a OGRSpatialReference.
        \return
    */
    std::shared_ptr<OGRCoordinateTransformation> to(GlobalCS::Ptr other);
protected:
    GlobalCS();
    GlobalCS(const core::IDGenBase*);
    friend class odb::access;

    /// used by both projection and geographic coordinate systems.
    #pragma db type("TEXT")
    OGRSpatialReference frame_;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_ */
