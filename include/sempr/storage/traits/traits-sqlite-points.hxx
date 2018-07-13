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
            const PointCloud::Point* p = reinterpret_cast<const PointCloud::Point*>(b.data());
            v.assign(p, (p + n / sizeof(PointCloud::Point)));
        }
        else
        {
            v.clear();
        }
    }

    // Create the DB image from the value
    static void set_image (details::buffer& b, std::size_t& n, bool& is_null,  std::vector<PointCloud::Point> const& v)
    {
        is_null = false;

        n = v.size() * sizeof(PointCloud::Point);

        if(n > b.capacity())
        {
            b.capacity(n);
        }

        if (n != 0)
        {
            std::memcpy(b.data(), &v.front(), n);
        }
    }
};

} /* sqlite */

} /* odb */

#endif /* end of include guard: SEMPR_TRAITS_SQLITE_POINTS_HXX */
