#ifndef SEMPR_ENTITY_SPATIAL_LINEAL_HPP_
#define SEMPR_ENTITY_SPATIAL_LINEAL_HPP_

#include <sempr/entity/spatial/Geometry.hpp>
#include <geos/geom/Lineal.h>

namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
    Interface for lineal geometries like LineString or LinearRing
*/
#pragma db object
class Lineal : public Geometry {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Lineal>;

    virtual const geom::Lineal* getGeometry();

    virtual ~Lineal();
protected:
    Lineal(const core::IDGenBase* idgen);

    virtual geom::Lineal* getGeometry() const;

private:
    friend class odb::access;

    Lineal();
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LINEAL_HPP_ */
