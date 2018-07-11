#ifndef SEMPR_TRAITS_SQLITE_POINTS_HXX
#define SEMPR_TRAITS_SQLITE_POINTS_HXX

#include <odb/sqlite/traits.hxx>
#include <sempr/entity/PointCloud.hpp>

#include <cpl_conv.h>

#include <cstddef> // std::size_t
#include <cstring> // std::strncmp, std::memcpy
#include <sstream>
#include <iostream>
#include <memory>

#include <type_traits>


// This is really hacky stuff :(

namespace odb { namespace sqlite {

using sempr::entity::PointCloud;

/*
template <>
class value_traits<PointCloud::Points*, id_blob>     // id_blob is the SQLite type for binary blobs ...
{
public:
    typedef PointCloud::Points* value_type;
    typedef PointCloud::Points* query_type;
    typedef details::buffer image_type;

    // Make the value from the image (from SQL)
    static void set_value (PointCloud::Points*& p, const details::buffer& b, std::size_t n, bool is_null)
    {
        if (!is_null)
        {
            uint64_t num, c_ptr;

            std::cout << "loading the pc traits" << std::endl;

            num = n / (sizeof(unsigned char) + sizeof(double) * 3);         // this should be the correct number of points?

            p = new PointCloud::Points();
            p->points.reserve(num);
            p->colors.reserve(num);
            
            std::shared_ptr<char> tmp(new char[n], std::default_delete<char[]>());
            std::memcpy (tmp.get(), b.data(), n);

            c_ptr = num * sizeof(double) * 3;                       // skip the points?
            
            std::memcpy(p->points.data(), tmp.get(), c_ptr );
            std::memcpy(p->colors.data(), (tmp.get() + c_ptr), 3 * num );
        }
        else
        {
            p = NULL;
        }
    }

    // Make the image from the value (to SQL)
    static void set_image (details::buffer& b, std::size_t& n, bool& is_null,  PointCloud::Points* const& p)
    {
        if (p == NULL) {
            is_null = true;
            n = 0;
            return;
        }

        is_null = false;
        uint64_t c_ptr, num;

        num = p->points.size();
        n = num * (1 + sizeof(double));      // get the numbers of points, so we can store them

        // store the points ((x, y, z), ...), ((r, g, b), ...)
        if (n > b.capacity())
                b.capacity(n);
        //THIS IS TODO !
        c_ptr = num * sizeof(double);

        std::shared_ptr<char> tmp(new char[n], std::default_delete<char[]>());
        std::memcpy(tmp.get(), p->points.data(), c_ptr);
        std::memcpy((tmp.get() + c_ptr), p->colors.data(), (num * 3));

        std::memcpy (b.data(), tmp.get(), n);
    }
};
*/


template <>
class value_traits<PointCloud::Points, id_blob>     // id_blob is the SQLite type for binary blobs ...
{
public:
    typedef PointCloud::Points value_type;
    typedef PointCloud::Points query_type;
    typedef details::buffer image_type;

    // Make the value from the image (from SQL)
    static void set_value (PointCloud::Points& p, const details::buffer& b, std::size_t n, bool is_null)
    {
        if (!is_null)
        {
            uint64_t num, c_ptr;

            std::cout << "loading the pc traits" << std::endl;

            num = n / (sizeof(unsigned char) + sizeof(double));         // this should be the correct number of points?

            //p.points.reserve(num);
            //p.colors.reserve(num);

            c_ptr = num * sizeof(double);                       // skip the points?

            //std::memcpy(p.points.data(), b.data(), c_ptr );
            //std::memcpy(p.colors.data(), (b.data() + c_ptr), num );
            p.points = std::vector<double> (b.data(), (b.data() + c_ptr - 1));
            std::cout << "loading p" << std::endl;
            p.colors = std::vector<unsigned char> ((b.data() + c_ptr), (b.data() + n - 1));
            std::cout << "loading c" << std::endl;
        }
        else
        {
        }
    }

    // Make the image from the value (to SQL)
    static void set_image (details::buffer& b, std::size_t& n, bool& is_null,  PointCloud::Points const& p)
    {
        if (p.points.size() == 0) {
            is_null = true;
            n = 0;
            return;
        }

        is_null = false;
        uint64_t c_ptr, num;

        num = p.points.size();
        n = num * (1 + sizeof(double));      // get the numbers of points, so we can store them

        // store the points ((x, y, z), ...), ((r, g, b), ...)
        if (n > b.capacity())
                b.capacity(n);
        //THIS IS TODO !
        c_ptr = num * sizeof(double);

        std::memcpy(b.data(), p.points.data(), c_ptr);
        std::memcpy((b.data() + c_ptr), p.colors.data(), num);
    }
};

} /* sqlite */

} /* odb */

#endif /* end of include guard: SEMPR_TRAITS_SQLITE_POINTS_HXX */
