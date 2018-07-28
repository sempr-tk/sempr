#include <sempr/entity/spatial/filter/LocalTransformationFilter.hpp>
#include <cmath>

namespace sempr {

LocalTransformationFilter::LocalTransformationFilter(const Eigen::Affine3d& aff) :
    mat_(aff),
    done_(false),
    changed_(false)
{
}

void LocalTransformationFilter::filter_rw(geom::CoordinateSequence& seq, std::size_t i)
{
    for(;i < seq.getSize(); i++)
    {
        //intern copy of every point. I hope it get optimized by the compiler.
        geom::Coordinate coord = seq.getAt(i);
        filter_rw(coord);
        seq.setAt(coord, i);
    }
}

void LocalTransformationFilter::filter_rw(geom::Coordinate& coordinate) const
{
    //check if coordinate is 3D or 2D
    if(!std::isnan(coordinate.z))
    {
        //coordinate is 3D
        Eigen::Vector3d vec3D(coordinate.x, coordinate.y, coordinate.z);

        Eigen::Vector3d tranformed3D = mat_ * vec3D;

        coordinate.x = tranformed3D.x();
        coordinate.y = tranformed3D.y();
        coordinate.z = tranformed3D.z();
    }
    else
    {
        //otherwise 2d
        Eigen::Affine2d mat2D = Eigen::Translation2d(mat_.translation().topRows<2>()) * mat_.linear().topLeftCorner<2,2>();

        Eigen::Vector2d vec2D(coordinate.x, coordinate.y);

        Eigen::Vector2d tranformed2D = mat2D * vec2D;

        coordinate.x = tranformed2D.x();
        coordinate.y = tranformed2D.y();
    }
}

void LocalTransformationFilter::filter_ro(const geom::CoordinateSequence& seq, std::size_t i)
{
    // do nothing
    throw TransformException("No read-only filter for a local transformation!");
}

bool LocalTransformationFilter::isDone() const
{
    return done_;
}

bool LocalTransformationFilter::isGeometryChanged() const
{
    return changed_;
}


} /* sempr */
