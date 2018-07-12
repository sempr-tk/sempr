#ifndef SEMPR_TRAITS_SQLITE_POINTS_HXX
#define SEMPR_TRAITS_SQLITE_POINTS_HXX

#include <odb/sqlite/traits.hxx>
#include <sempr/entity/PointCloud.hpp>

#include <cstddef> // std::size_t
#include <cstring> // std::strncmp, std::memcpy
#include <vector>

#include <type_traits>

namespace odb { namespace sqlite {

using sempr::entity::PointCloud;


/**
 *  Stores the vectors from the Pointcloud::Points struct into the SQLite DB.
 */
/*
template <>
class value_traits<PointCloud::Points, id_blob>
{
public:
    typedef PointCloud::Points value_type;
    typedef PointCloud::Points query_type;
    typedef details::buffer image_type;

    // Make the value from the DB image
    static void set_value (PointCloud::Points& p, const details::buffer& b, std::size_t n, bool is_null)
    {
        if (!is_null)
        {
            uint64_t num;
            uint64_t c_ptr;

            num = n / (sizeof(unsigned char) + sizeof(double));
            c_ptr = num * sizeof(double);

            p.points = std::vector<double> (b.data(), (b.data() + c_ptr - 1));
            p.colors = std::vector<unsigned char> ((b.data() + c_ptr), (b.data() + n - 1));
        }
        else
        {
            // What to do else?
        }
    }

    // Create the DB image from the value
    static void set_image (details::buffer& b, std::size_t& n, bool& is_null,  PointCloud::Points const& p)
    {
        uint64_t num;
        uint64_t c_ptr;

        if (p.points.size() == 0) {
            is_null = true;
            n = 0;
            return;
        }

        is_null = false;

        num = p.points.size();
        n = num * (1 + sizeof(double));

        if (n > b.capacity())
                b.capacity(n);
        c_ptr = num * sizeof(double);

        std::memcpy(b.data(), p.points.data(), c_ptr);
        std::memcpy((b.data() + c_ptr), p.colors.data(), num);
    }
};
*/

template <>
class value_traits<std::vector<PointCloud::Point>, id_blob>
{
public:
    typedef std::vector<PointCloud::Point> value_type;
    typedef std::vector<PointCloud::Point> query_type;
    typedef details::buffer image_type;

    // Make the value from the DB image
    static void set_value (std::vector<PointCloud::Point>& v, const details::buffer& b, std::size_t n, bool is_null)
    {
        if (!is_null)
        {
            const PointCloud::Point* p = reinterpret_cast<const PointCloud::Point*> (b.data ());
            v.assign (p, (p + n / sizeof(PointCloud::Point)));
        }
        else
        {
            // What to do else?
            v.clear();
        }
    }

    // Create the DB image from the value
    static void set_image (details::buffer& b, std::size_t& n, bool& is_null,  std::vector<PointCloud::Point> const& v)
    {
        is_null = false;

        n = v.size() * sizeof(PointCloud::Point);

        if (n > b.capacity())
        {
            b.capacity(n);
        }

        if (n != 0)
        {
            std::memcpy (b.data(), &v.front(), n);
        }
    }
};

} /* sqlite */

} /* odb */

#endif /* end of include guard: SEMPR_TRAITS_SQLITE_POINTS_HXX */
