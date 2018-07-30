#ifndef SEMPR_ENTITY_SPATIAL_LINEARRING_HPP_
#define SEMPR_ENTITY_SPATIAL_LINEARRING_HPP_

#include <sempr/entity/spatial/Lineal.hpp>
#include <geos/geom/LinearRing.h>

#include <geos/geom/CoordinateArraySequenceFactory.h>

namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
    Container class for GEOM LineRing
*/
#pragma db object
class LinearRing : public Lineal {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<LinearRing>;

    LinearRing();
    LinearRing(const core::IDGenBase*);
    virtual ~LinearRing();

    const geom::LinearRing* getGeometry() override;

    void setGeometry(geom::LinearRing* geometry);

    void setCoordinates(std::vector<geom::Coordinate>& coordinates); //no const parameter because of geos::geom!

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    LinearRing::Ptr clone() const;

protected:
    geom::LinearRing* geometry() override;

private:
    friend class odb::access;

    #pragma db type("TEXT")
    geom::LinearRing* geometry_;

    virtual LinearRing* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LINEARRING_HPP_ */
