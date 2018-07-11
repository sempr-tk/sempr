#ifndef SEMPR_TRAITS_SQLITE_POINTCLOUD_HXX
#define SEMPR_TRAITS_SQLITE_POINTCLOUD_HXX

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

            num = n / (sizeof(unsigned char) + sizeof(double) * 3);         // this should be the correct number of points?

            std::shared_ptr<char> tmp(new char[n], std::default_delete<char[]>());
            std::memcpy (tmp.get(), b.data(), n);

            std::vector<double> tmpPts(num);
            std::vector<unsigned char> tmpCol(num);
            c_ptr = num * sizeof(double) * 3;                       // skip the points?
            //std::memcpy (tmpPts.get(), tmp.get(), c_ptr);
            std::fill(tmpPts.begin(), tmpPts.end(), b.data());
            //std::fill_n(tmpPts.get(), (num * 3), &b.data());
            std::cout << "got points" << std::endl;


            //std::memcpy (tmpCol.get(), (tmp.get() + c_ptr), num * 3);
            std::fill(tmpCol.begin(), tmpCol.end(), b.data()[c_ptr]);
            //std::fill_n(tmpCol.get(), (num * 3), &b.data()[c_ptr]);
            std::cout << "got colors" << std::endl;

            pc.setPointsWithColor(tmpPts, tmpCol, num);
        }
        else
        {
        }
    }

    // Make the image from the value (to SQL)
    static void set_image (details::buffer& b, std::size_t& n, bool& is_null,  const PointCloud::Points& p)
    {
        /*
        if (pc == NULL) {
            is_null = true;
            n = 0;
            return;
        }
        */

        is_null = false;
        uint64_t c_ptr, num;

        num = p.m_pc->number();
        n = 3 * num * (1 + sizeof(double));      // get the numbers of points, so we can store them


        // store the points ((x, y, z), ...), ((r, g, b), ...)
        if (n > b.capacity())
                b.capacity(n);
        //THIS IS TODO !
        c_ptr = num * sizeof(double) * 3;

        std::vector<double> points = p.mm_points;
        std::vector<unsigned char> colors = p.mm_colors;
        std::shared_ptr<char> tmp(new char[n], std::default_delete<char[]>());
        std::memcpy(tmp.get(), points.data(), c_ptr);
        std::memcpy((tmp.get() + c_ptr), colors.data(), (num * 3));

        //std::fill(b.data(), pc.points().get()[0] + (num * 3), pc.points().get()[0]);
        //std::fill_n(b.data(), n, tmp.get());
        std::memcpy (b.data(), tmp.get(), n);
        std::cout << "save points" << std::endl;
        //std::fill(b.data()[c_ptr], pc.colors().get()[0] + (num * 3), pc.colors().get()[0]);
        //std::fill(b.data(), pc.colors().get()[0] + (num * 3), pc.colors().get()[0]);
        //std::fill_n(b.data(), num * 3, pc.colors().get());
        //std::memcpy ((b.data())[c_ptr], pc.colors(), num * 3);
        std::cout << "save colors" << std::endl;

    }
};

} /* sqlite */

} /* odb */


#endif /* end of include guard: SEMPR_TRAITS_SQLITE_POINTCLOUD_HXX */
