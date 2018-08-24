#ifndef SEMPR_TRAITS_SQLITE_POINTCLOUD_HXX
#define SEMPR_TRAITS_SQLITE_POINTCLOUD_HXX

#include <odb/sqlite/traits.hxx>

#include <sempr/entity/spatial/PointCloud.hpp>

#include <cstddef> // std::size_t
#include <cstring> // std::strncmp, std::memcpy
#include <sstream>
#include <iostream>

namespace odb { namespace sqlite {


template <>
class value_traits<sempr::entity::ChannelVariant, id_text>
{
public:
    typedef sempr::entity::ChannelVariant value_type;
    typedef sempr::entity::ChannelVariant query_type;
    typedef details::buffer image_type;

    static void
    set_value(value_type& channel, const image_type& b, std::size_t n, bool is_null)
    {
        /*
        if (!is_null) {
            std::string tmp(b.data(), n);
            std::stringstream ss(tmp);

            std::vector<double> ch;

            double value;
            while (ss >> value)
                ch.emplace_back(value);

            channel = sempr::entity::Channel(ch);
        } 
        */
    }

    static void
    set_image(image_type& b, std::size_t& n, bool& is_null, const value_type& channel)
    {

        //std::type_info info = channel.type();

        /*
        is_null = false;
        std::stringstream ss;

        //ss << "(";
        for (std::size_t i=0; i < channel.size(); i++)
        {
            ss << channel[i];

            if(i+1 != channel.size())
                ss << " ";  // ", "
        }
        //ss << ")";

        std::string tmp = ss.str();
        n = tmp.size();

        if (b.capacity() < n) {
            b.capacity(n);
        }
        std::memcpy(b.data(), tmp.data(), n);
        */
    }
};


} /* sqlite */

} /* odb */


#endif /* end of include guard: SEMPR_TRAITS_SQLITE_POINTCLOUD_HXX */
