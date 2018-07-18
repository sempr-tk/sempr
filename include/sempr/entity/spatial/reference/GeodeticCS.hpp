#ifndef SEMPR_ENTITY_SPATIAL_GEODETICS_HPP_
#define SEMPR_ENTITY_SPATIAL_GEODETICS_HPP_

#include <sempr/entity/spatial/reference/GlobalCS.hpp>
#include <sempr/entity/spatial/filter/GeodeticFilter.hpp>
//#include <ogr_spatialref.h>

namespace sempr { namespace entity {

/**
    A geodetic reference system in WGS84.
    It is also a root of transformations.
*/
#pragma db object
class GeodeticCS : public GlobalCS {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<GeodeticCS>;

    GeodeticCS();
    GeodeticCS(const core::IDGenBase*);

    FilterList to(const GlobalCS::Ptr other) override;
/*
protected:
    std::shared_ptr<geos::geom::CoordinateFilter> froward() const override;
    std::shared_ptr<geos::geom::CoordinateFilter> reverse() const override;
*/
private:
    friend class odb::access;
};


}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEODETICS_HPP_ */
