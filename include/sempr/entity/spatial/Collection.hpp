#ifndef SEMPR_ENTITY_SPATIAL_COLLECTION_HPP_
#define SEMPR_ENTITY_SPATIAL_COLLECTION_HPP_

#include <sempr/entity/spatial/Geometry.hpp>
#include <geos/geom/GeometryCollection.h>

namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
    Interface for geometry collection entities like a GeometryCollection or MultiPoint
*/
#pragma db object
class Collection : public Geometry {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Collection>;

    virtual const geom::GeometryCollection* getGeometry() const override;

    virtual ~Collection();
protected:
    Collection(const core::IDGenBase* idgen);

    virtual geom::GeometryCollection* getGeometryMut() override;

private:
    friend class odb::access;
    Collection();

};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_COLLECTION_HPP_ */
