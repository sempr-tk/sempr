#ifndef SEMPR_ENTITY_SPATIAL_GEOCOLLECTION_HPP_
#define SEMPR_ENTITY_SPATIAL_GEOCOLLECTION_HPP_

#include <sempr/entity/spatial/Geometry.hpp>

namespace sempr { namespace entity {

/**
    Container class for OGRGeometryCollection
*/
#pragma db object
class GeometryCollection : public Geometry {
public:
    using Ptr = std::shared_ptr<GeometryCollection>;
    ENTITY_DEFAULT_EVENT_METHODS(GeometryCollection, Geometry);

    GeometryCollection();
    GeometryCollection(const core::IDGenBase*);
    virtual ~GeometryCollection();

    OGRGeometryCollection* geometry() override;

private:
    friend class odb::access;
    #pragma db type("TEXT")
    OGRGeometryCollection* geometry_;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEOCOLLECTION_HPP_ */
