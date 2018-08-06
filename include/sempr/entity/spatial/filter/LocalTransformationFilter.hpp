#ifndef SEMPR_ENTITY_SPATIAL_LOCALTRANSFORMATIONFILTER_HPP_
#define SEMPR_ENTITY_SPATIAL_LOCALTRANSFORMATIONFILTER_HPP_

//#include <ogr_spatialref.h>
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <geos/geom/CoordinateSequenceFilter.h>
#include <geos/geom/CoordinateSequence.h>
#include <exception>
#include <string>

// weeeeeell... it's not an entity. more like... util?
namespace sempr {

namespace geom = geos::geom;

/** Exception that may be thrown during transformToCS. */
class TransformException : public std::exception
{
    private:
    std::string message_;

    public:
    explicit TransformException(const std::string &m) : message_(m) {}
    virtual const char *what() const throw()
    {
        return message_.c_str();
    }
};



class LocalTransformationFilter : public geom::CoordinateSequenceFilter
{
public:
    /**
        Create a transformation defined by the Affine3d matrix.
    */
    LocalTransformationFilter(const Eigen::Affine3d& aff);

    void filter_rw(geom::CoordinateSequence& seq, std::size_t i) override;

    void filter_ro(const geom::CoordinateSequence& seq, std::size_t i) override;

    bool isDone() const override;

    bool isGeometryChanged() const override;

private:
    /// ref to const transform given on initialization
    const Eigen::Affine3d& mat_;

    bool changed_;

    void filter_rw(geom::Coordinate& coordinate) const;
};

} /* sempr */


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LOCALTRANSFORMATIONFILTER_HPP_ */
