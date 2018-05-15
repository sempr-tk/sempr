#ifndef SEMPR_TRAITS_SQLITE_EIGEN_HXX
#define SEMPR_TRAITS_SQLITE_EIGEN_HXX

#include <odb/sqlite/traits.hxx>

#include <cstddef> // std::size_t
#include <cstring> // std::strncmp, std::memcpy
#include <sstream>
#include <iostream>

#include <Eigen/Geometry>

namespace odb { namespace sqlite {


template <>
class value_traits<Eigen::Affine3d, id_text>
{
public:
    typedef Eigen::Affine3d value_type;
    typedef Eigen::Affine3d query_type;
    typedef details::buffer image_type;

    static void
    set_value(value_type& affine, const image_type& b, std::size_t n, bool is_null)
    {
        if (!is_null) {
            std::string tmp(b.data(), n);
            std::stringstream ss(tmp);
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    ss >> affine(i,j);  // set elements of transformation
                }
            }
        } else {
            affine.setIdentity(); // null-transform? shouldnt happen. but if, use identity.
        }

        // called multiple times per load. why?
        // std::cout << "set_value: " << affine << " is null: " << is_null << " size: " << n << '\n';
    }

    static void
    set_image(image_type& b, std::size_t& n, bool& is_null, const value_type& affine)
    {
        // std::cout << "set_image" << '\n';
        is_null = false;
        std::stringstream ss;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                ss << affine(i,j) << " ";
            }
        }
        std::string tmp = ss.str();
        n = tmp.size();

        if (b.capacity() < n) {
            b.capacity(n);
        }
        std::memcpy(b.data(), tmp.data(), n);
    }
};


} /* sqlite */

} /* odb */


#endif /* end of include guard: SEMPR_TRAITS_SQLITE_EIGEN_HXX */
