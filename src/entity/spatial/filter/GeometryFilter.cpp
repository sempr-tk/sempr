#include <sempr/entity/spatial/filter/GeometryFilter.hpp>

#include <cmath>
#include <limits>

namespace sempr {

EnvelopeFilter::EnvelopeFilter()
{
    min_.x = + std::numeric_limits<double>::max();
    min_.y = + std::numeric_limits<double>::max();
    min_.z = + std::numeric_limits<double>::max();

    max_.x = - std::numeric_limits<double>::max();
    max_.y = - std::numeric_limits<double>::max();
    max_.z = - std::numeric_limits<double>::max();

    done_ = false;
    changed_ = false;
}

void EnvelopeFilter::filter_rw(geom::CoordinateSequence& seq, std::size_t i)
{
    // do nothing else like the read only filter.
    filter_ro(seq, i);
}

void EnvelopeFilter::filter_ro(const geom::CoordinateSequence& seq, std::size_t i)
{
    // a sequence shall only contain 2d or 3d coordinates. No valid output if they are mixed!

    for(;i < seq.getSize(); i++)
    {
        //intern copy of every point. I hope it get optimized by the compiler.
        geom::Coordinate coord = seq.getAt(i);
        
        if (coord.x < min_.x)
            min_.x = coord.x;

        if (coord.x > max_.x)
            max_.x = coord.x;

        if (coord.y < min_.y)
            min_.y = coord.y;

        if (coord.y > max_.y)
            max_.y = coord.y;

        if (coord.z < min_.z)
            min_.z = coord.z;

        if (coord.z > max_.z)
            max_.z = coord.z;

        if (min_.z == std::numeric_limits<double>::max() || max_.z == std::numeric_limits<double>::min())
        {
            //only 2d:
            min_.z = std::numeric_limits<double>::quiet_NaN();
            max_.z = std::numeric_limits<double>::quiet_NaN();
        }

    }

    changed_ = false;
    done_ = true;
}

bool EnvelopeFilter::isDone() const
{
    return done_;
}

bool EnvelopeFilter::isGeometryChanged() const
{
    return changed_;
}

geom::Coordinate EnvelopeFilter::getMin() const
{
    return min_;
}

geom::Coordinate EnvelopeFilter::getMax() const
{
    return max_;
}


} /* sempr */
