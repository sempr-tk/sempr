#ifndef SEMPR_ENTITY_SPATIAL_LOCALTRANSFORMATION_HPP_
#define SEMPR_ENTITY_SPATIAL_LOCALTRANSFORMATION_HPP_

#include <ogr_spatialref.h>
#include <Eigen/Geometry>

// weeeeeell... it's not an entity. more like... util?
namespace sempr {

    /**
        This class implements an OGRCoordinateTransformation that simply
        applies an affine transformation to the given coordinates, without
        changing any other details (units, coordinate systems, ...). It is
        the equivalent of OGRCoordinateTransformations created to transform
        a geometry from one geographic or projected coordinate system into
        another, just for local coordinate systems that are not part of
        GDAL.
    */
    class LocalTransformation : public OGRCoordinateTransformation {
    public:
        /**
            Create a transformation defined by the Affine3d matrix.
        */
        LocalTransformation(const Eigen::Affine3d& aff);

        /**
            Get[Source|Target]CS both return NULL, as we don't transform between two OGRSpatialReferences, but our own LocalCS.
        */
        OGRSpatialReference* GetSourceCS() override;
        OGRSpatialReference* GetTargetCS() override;

        /**
            Apply transformation to the given points.
            \param nCount number of points
            \param x array of nCount X values, modified in place
            \param y array of nCount Y values, modified in place
            \param z array of nCount Z values, modified in place
            \return TRUE on success, FALSE if some points fail to transform -- but to be honest, it's a matrix multiplication, what can go wrong?
        */
        int Transform(int nCount, double* x, double* y, double* z = nullptr) override;

        /**
            Apply transformation to the given point, extended version.
            \param nCount number of points
            \param x array of nCount X values, modified in place
            \param y array of nCount Y values, modified in place
            \param z array of nCount Z values, modified in place
            \param pabSuccess array of per-point-success-flags.
            \return TRUE on success, FALSE if some points fail to transform -- but to be honest, it's a matrix multiplication, what can go wrong?
        */
        int TransformEx(int nCount, double* x, double* y, double* z = nullptr, int* pabSuccess = nullptr) override;

    private:
        /// ref to const transform given on initialization
        const Eigen::Affine3d& mat_;

        /**
            transform X Y only
        */
        void tf2D(int nCount, double* x, double* y, int* pabSuccess);

        /**
            transform X Y and Z
            (it really is Z, not M, not nullptr)
        */
        void tf3D(int nCount, double* x, double* y, double* z, int* pabSuccess);
    };

} /* sempr */


#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LOCALTRANSFORMATION_HPP_ */
