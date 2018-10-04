#ifndef SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_
#define SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_

#include <sempr/entity/spatial/reference/SpatialReference.hpp>
#include <geos/geom/CoordinateFilter.h>

namespace sempr { namespace entity {

/**
    Simple base class for ProjectionCS and GeographicCS, as both are root, and both need to
    be able to create a transformation to the other. It provides implementations of
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

    //from this to other
    virtual FilterList to(const GlobalCS::Ptr other) const;

    enum CardinalDirection
    {
        NORTH,
        EAST,
        SOUTH,
        WEST
    };

    /**
     * @brief Find the dimension that representate the cardinal direction.
     * 
     * @return std::size_t The dimension (X = 0, Y = 1, Z = 2)
     */
    virtual std::size_t directionDimension(const CardinalDirection& direction) const;

protected:
    GlobalCS();
    GlobalCS(const core::IDGenBase*);

    /**
     * @brief Forword transformation from geodetic CS (WGS84) to specific CS like projection or geocentric
     * 
     * @return std::shared_ptr<geos::geom::CoordinateFilter> 
     */
    virtual FilterPtr forward() const;

    // Allow forward call from childs
    inline FilterPtr forward(const GlobalCS::Ptr other) const { return other->forward(); }

    /**
     * @brief Reverse transformation from a specific CS like projection or geocentric to the general geodetic CS (WGS84)
     * 
     * @return std::shared_ptr<geos::geom::CoordinateFilter> 
     */
    virtual FilterPtr reverse() const;

    // Allow reverse call from childs
    inline FilterPtr reverse(const GlobalCS::Ptr other) const { return other->reverse(); }

private:
    friend class odb::access;

};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GLOBALCS_HPP_ */
