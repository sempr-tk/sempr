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
        
        if (!is_null) {
            std::string tmp(b.data(), n);
            std::stringstream ss(tmp);

            std::string type;
            ss >> type;     // read typeID


            if (type == "i8")
            {
                sempr::entity::Channel<int8_t> ch;

                int8_t value;
                while (ss >> value)
                    ch.add(value);

                channel = ch;
            }
            else if (type == "i16")
            {
                sempr::entity::Channel<int16_t> ch;

                int16_t value;
                while (ss >> value)
                    ch.add(value);

                channel = ch;
            }
            else if (type == "i32")
            {
                sempr::entity::Channel<int32_t> ch;

                int32_t value;
                while (ss >> value)
                    ch.add(value);

                channel = ch;
            }
            else if (type == "i64")
            {
                sempr::entity::Channel<int64_t> ch;

                int64_t value;
                while (ss >> value)
                    ch.add(value);

                channel = ch;
            }
            else if (type == "u8")
            {
                sempr::entity::Channel<uint8_t> ch;

                uint8_t value;
                while (ss >> value)
                    ch.add(value);

                channel = ch;
            }
            else if (type == "u16")
            {
                sempr::entity::Channel<uint16_t> ch;

                uint16_t value;
                while (ss >> value)
                    ch.add(value);

                channel = ch;
            }
            else if (type == "u32")
            {
                sempr::entity::Channel<uint32_t> ch;

                uint32_t value;
                while (ss >> value)
                    ch.add(value);

                channel = ch;
            }
            else if (type == "u64")
            {
                sempr::entity::Channel<uint64_t> ch;

                uint64_t value;
                while (ss >> value)
                    ch.add(value);

                channel = ch;
            }
            else if (type == "f")
            {
                sempr::entity::Channel<float> ch;

                float value;
                while (ss >> value)
                    ch.add(value);

                channel = ch;
            }
            else if (type == "d")
            {
                sempr::entity::Channel<double> ch;

                double value;
                while (ss >> value)
                    ch.add(value);

                channel = ch;
            }

        } 
        
    }

    static void
    set_image(image_type& b, std::size_t& n, bool& is_null, const value_type& channel)
    {
        is_null = false;
        std::stringstream ss;

        std::string type = findTypeID(channel);

        ss << type << " "; //write typeID

        ss << channel;  //use the stream operator of the variant that will use the stream operator of the channel

        std::string tmp = ss.str();
        n = tmp.size();

        if (b.capacity() < n) {
            b.capacity(n);
        }
        std::memcpy(b.data(), tmp.data(), n);
        
    }


    static void setVariant(const std::string& typeID, sempr::entity::ChannelVariant& var)
    {
        var = sempr::entity::Channel<double>();
    }

    static std::string findTypeID(const sempr::entity::ChannelVariant& variant)
    {

        try
        {
            boost::get< sempr::entity::Channel<int8_t> >(variant);
            return "i8";
        }
        catch (const boost::exception& ex) {}

        try
        {
            boost::get< sempr::entity::Channel<int16_t> >(variant);
            return "i16";
        }
        catch (const boost::exception& ex) {}

        try
        {
            boost::get< sempr::entity::Channel<int32_t> >(variant);
            return "i32";
        }
        catch (const boost::exception& ex) {}

        try
        {
            boost::get< sempr::entity::Channel<int64_t> >(variant);
            return "i64";
        }
        catch (const boost::exception& ex) {}

        try
        {
            boost::get< sempr::entity::Channel<uint8_t> >(variant);
            return "u8";
        }
        catch (const boost::exception& ex) {}

        try
        {
            boost::get< sempr::entity::Channel<uint16_t> >(variant);
            return "u16";
        }
        catch (const boost::exception& ex) {}

        try
        {
            boost::get< sempr::entity::Channel<uint32_t> >(variant);
            return "u32";
        }
        catch (const boost::exception& ex) {}

        try
        {
            boost::get< sempr::entity::Channel<uint64_t> >(variant);
            return "u64";
        }
        catch (const boost::exception& ex) {}

        try
        {
            boost::get< sempr::entity::Channel<float> >(variant);
            return "f";
        }
        catch (const boost::exception& ex) {}

        try
        {
            boost::get< sempr::entity::Channel<double> >(variant);
            return "d";
        }
        catch (const boost::exception& ex) {}


        return "";
    }


};


} /* sqlite */

} /* odb */


#endif /* end of include guard: SEMPR_TRAITS_SQLITE_POINTCLOUD_HXX */
