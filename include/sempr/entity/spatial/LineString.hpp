#ifndef SEMPR_ENTITY_SPATIAL_LINESTRING_HPP_
#define SEMPR_ENTITY_SPATIAL_LINESTRING_HPP_

#include <sempr/entity/spatial/Lineal.hpp>
#include <geos/geom/LineString.h>
#include <geos/geom/CoordinateArraySequenceFactory.h>

namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
    Container class for GEOM LineString
*/
#pragma db object
class LineString : public Lineal {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<LineString>;

    LineString();
    LineString(const core::IDGenBase*);
    virtual ~LineString();

    const geom::LineString* getGeometry() override;

    void setGeometry(geom::LineString* geometry);

    void setCoordinates(std::vector<geom::Coordinate>& coordinates); //no const parameter because of geos::geom!

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    LineString::Ptr clone() const;

protected:
    geom::LineString* getGeometry() const override;

private:
    friend class odb::access;

    #pragma db type("TEXT")
    geom::LineString* geometry_;

    virtual LineString* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LINESTRING_HPP_ */
