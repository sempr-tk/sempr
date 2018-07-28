#ifndef SEMPR_ENTITY_SPATIAL_GEOMETRYFILTER_HPP_
#define SEMPR_ENTITY_SPATIAL_GEOMETRYFILTER_HPP_

#include <cstddef>

#include <geos/geom/CoordinateSequenceFilter.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/Coordinate.h>

// weeeeeell... it's not an entity. more like... util?
namespace sempr {

namespace geom = geos::geom;


class EnvelopeFilter : public geom::CoordinateSequenceFilter 
{
public:
    /**
        Create a transformation defined by the Affine3d matrix.
    */
    EnvelopeFilter();

    void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;

    void filter_ro(const geom::CoordinateSequence& seq, std::size_t i) override;

    bool isDone() const override;

    bool isGeometryChanged() const override;

    geom::Coordinate getMin() const;
    geom::Coordinate getMax() const;

private:
    geom::Coordinate min_;
    geom::Coordinate max_;

    bool done_;
    bool changed_;
};



} /* sempr */


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_GEOMETRYFILTER_HPP_ */
