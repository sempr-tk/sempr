#ifndef SEMPR_TRAITS_SQLITE_POINTCLOUD_HXX
#define SEMPR_TRAITS_SQLITE_POINTCLOUD_HXX

#include <odb/sqlite/traits.hxx>

#include <sempr/entity/spatial/PointCloud.hpp>

#include <cstddef> // std::size_t
#include <cstring> // std::strncmp, std::memcpy
#include <sstream>
#include <iostream>

#include <assert.h> 

namespace odb { namespace sqlite {



/**
 * This class will convert a channel to a BLOB value.
 * The first byte defines the type of the channel.
 * 
 * Note: Only use this on the same system architecture (like little or big endian)!
 */
template <>
class value_traits<sempr::entity::ChannelVariant, id_blob>
{
public:
    typedef sempr::entity::ChannelVariant value_type;
    typedef sempr::entity::ChannelVariant query_type;
    typedef details::buffer image_type;

    enum variantType
    {
        i8,
        i16,
        i32,
        i64,
        u8,
        u16,
        u32,
        u64,
        f,
        d
    };

    static void
    set_value(value_type& channel, const image_type& b, std::size_t n, bool is_null)
    {
        if (!is_null) 
        {
            char type = b.data()[0];

            std::size_t sizeValues = n - 1; //first byte for type info

            switch (type)
            {
                case i8:
                {
                    std::size_t numEle = sizeValues / sizeof(int8_t);

                    assert(sizeValues % sizeof(int8_t) == 0);

                    sempr::entity::Channel<int8_t> ch;
                    ch.resize(numEle);

                    memcpy(ch.data(), b.data()+1, sizeValues);

                    channel = ch;

                    break;
                }

                case i16:
                {
                    std::size_t numEle = sizeValues / sizeof(int16_t);

                    assert(sizeValues % sizeof(int16_t) == 0);

                    sempr::entity::Channel<int16_t> ch;
                    ch.resize(numEle);

                    memcpy(ch.data(), b.data()+1, sizeValues);

                    channel = ch;

                    break;
                }

                case i32:
                {
                    std::size_t numEle = sizeValues / sizeof(int32_t);

                    assert(sizeValues % sizeof(int32_t) == 0);

                    sempr::entity::Channel<int32_t> ch;
                    ch.resize(numEle);

                    memcpy(ch.data(), b.data()+1, sizeValues);

                    channel = ch;

                    break;
                }

                case i64:
                {
                    std::size_t numEle = sizeValues / sizeof(int64_t);

                    assert(sizeValues % sizeof(int64_t) == 0);

                    sempr::entity::Channel<int64_t> ch;
                    ch.resize(numEle);

                    memcpy(ch.data(), b.data()+1, sizeValues);

                    channel = ch;

                    break;
                }

                case u8:
                {
                    std::size_t numEle = sizeValues / sizeof(uint8_t);

                    assert(sizeValues % sizeof(uint8_t) == 0);

                    sempr::entity::Channel<uint8_t> ch;
                    ch.resize(numEle);

                    memcpy(ch.data(), b.data()+1, sizeValues);

                    channel = ch;

                    break;
                }

                case u16:
                {
                    std::size_t numEle = sizeValues / sizeof(uint16_t);

                    assert(sizeValues % sizeof(uint16_t) == 0);

                    sempr::entity::Channel<uint16_t> ch;
                    ch.resize(numEle);

                    memcpy(ch.data(), b.data()+1, sizeValues);

                    channel = ch;

                    break;
                }

                case u32:
                {
                    std::size_t numEle = sizeValues / sizeof(uint32_t);

                    assert(sizeValues % sizeof(uint32_t) == 0);

                    sempr::entity::Channel<uint32_t> ch;
                    ch.resize(numEle);

                    memcpy(ch.data(), b.data()+1, sizeValues);

                    channel = ch;

                    break;
                }

                case u64:
                {
                    std::size_t numEle = sizeValues / sizeof(uint64_t);

                    assert(sizeValues % sizeof(uint64_t) == 0);

                    sempr::entity::Channel<uint64_t> ch;
                    ch.resize(numEle);

                    memcpy(ch.data(), b.data()+1, sizeValues);

                    channel = ch;

                    break;
                }

                case f:
                {
                    std::size_t numEle = sizeValues / sizeof(float);

                    assert(sizeValues % sizeof(float) == 0);

                    sempr::entity::Channel<float> ch;
                    ch.resize(numEle);

                    memcpy(ch.data(), b.data()+1, sizeValues);

                    channel = ch;

                    break;
                }

                case d:
                {
                    std::size_t numEle = sizeValues / sizeof(double);

                    assert(sizeValues % sizeof(double) == 0);

                    sempr::entity::Channel<double> ch;
                    ch.resize(numEle);

                    memcpy(ch.data(), b.data()+1, sizeValues);

                    channel = ch;

                    break;
                }
            }



        } 
    }

    static void
    set_image(image_type& b, std::size_t& n, bool& is_null, const value_type& channel)
    {

        char type;
        void* data;
        std::size_t size;
        getVariantData(channel, type, data, size);

        if (data != NULL)
        {
            is_null = false;
            n = size + 1;   // one byte for the type

            if (b.capacity() < n) {
                b.capacity(n);
            }

            b.data()[0] = type;   //set type info

            //copy data to image
            std::memcpy(b.data()+1, data, size);
        }
        else
        {
            is_null = true;
            n = 0;
        }

    }

    static void getVariantData(const sempr::entity::ChannelVariant& variant, char& type, void*& data, std::size_t& size)
    {
        data = NULL;

        try
        {
            auto var = boost::get< sempr::entity::Channel<int8_t> >(variant);
            data = var.data();
            size = var.size() * sizeof(int8_t);
            type = char(i8);
            return;
        }
        catch (const boost::exception& ex) {}

        try
        {
            auto var = boost::get< sempr::entity::Channel<int16_t> >(variant);
            data = var.data();
            size = var.size() * sizeof(int16_t);
            type = char(i16);
            return;
        }
        catch (const boost::exception& ex) {}

        try
        {
            auto var = boost::get< sempr::entity::Channel<int32_t> >(variant);
            data = var.data();
            size = var.size() * sizeof(int32_t);
            type = char(i32);
            return;
        }
        catch (const boost::exception& ex) {}

        try
        {
            auto var = boost::get< sempr::entity::Channel<int64_t> >(variant);
            data = var.data();
            size = var.size() * sizeof(int64_t);
            type = char(i64);
            return;
        }
        catch (const boost::exception& ex) {}

        try
        {
            auto var = boost::get< sempr::entity::Channel<uint8_t> >(variant);
            data = var.data();
            size = var.size() * sizeof(uint8_t);
            type = char(u8);
            return;
        }
        catch (const boost::exception& ex) {}

        try
        {
            auto var = boost::get< sempr::entity::Channel<uint16_t> >(variant);
            data = var.data();
            size = var.size() * sizeof(uint16_t);
            type = char(u16);
            return;
        }
        catch (const boost::exception& ex) {}

        try
        {
            auto var = boost::get< sempr::entity::Channel<uint32_t> >(variant);
            data = var.data();
            size = var.size() * sizeof(uint32_t);
            type = char(u32);
            return;
        }
        catch (const boost::exception& ex) {}

        try
        {
            auto var = boost::get< sempr::entity::Channel<uint64_t> >(variant);
            data = var.data();
            size = var.size() * sizeof(uint64_t);
            type = char(u64);
            return;
        }
        catch (const boost::exception& ex) {}

        try
        {
            auto var = boost::get< sempr::entity::Channel<float> >(variant);
            data = var.data();
            size = var.size() * sizeof(float);
            type = char(f);
            return;
        }
        catch (const boost::exception& ex) {}

        try
        {
            auto var = boost::get< sempr::entity::Channel<double> >(variant);
            data = var.data();
            size = var.size() * sizeof(double);
            type = char(d);
            return;
        }
        catch (const boost::exception& ex) {}
    }
};




/**
 * This class will convert a channel to a TEXT value.
 * Every element is separted by a whitespace.
 * The first element defines the type of the channel
 */
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
