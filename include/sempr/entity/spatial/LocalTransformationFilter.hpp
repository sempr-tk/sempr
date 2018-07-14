#ifndef SEMPR_ENTITY_SPATIAL_LOCALTRANSFORMATION_HPP_
#define SEMPR_ENTITY_SPATIAL_LOCALTRANSFORMATION_HPP_

//#include <ogr_spatialref.h>
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <geos/geom/CoordinateFilter.h>
#include <geos/geom/Coordinate.h>

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


    /**
        This class implements an OGRCoordinateTransformation that simply
        applies an affine transformation to the given coordinates, without
        changing any other details (units, coordinate systems, ...). It is
        the equivalent of OGRCoordinateTransformations created to transform
        a geometry from one geographic or projected coordinate system into
        another, just for local coordinate systems that are not part of
        GDAL.

        Note: I think it could also be done by a CoordinateSequenceFilter in a bit more effective Way. (have to try it!)
    */
    class LocalTransformation : public geom::CoordinateFilter {
    public:
        /**
            Create a transformation defined by the Affine3d matrix.
        */
        LocalTransformation(const Eigen::Affine3d& aff);

        void filter_rw(geom::Coordinate* coordinate) const;

        void filter_ro(const geom::Coordinate* coordinate);

    private:
        /// ref to const transform given on initialization
        const Eigen::Affine3d& mat_;
    };

} /* sempr */


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LOCALTRANSFORMATION_HPP_ */
