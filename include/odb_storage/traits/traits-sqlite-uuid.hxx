#ifndef SEMPR_TRAITS_SQLITE_UUID_HXX
#define SEMPR_TRAITS_SQLITE_UUID_HXX

#include <odb/sqlite/traits.hxx>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>

#include <cstddef> // std::size_t
#include <cstring> // std::strncmp, std::memcpy
#include <sstream>
#include <iostream>

namespace odb { namespace sqlite {


template <>
class value_traits<boost::uuids::uuid, id_text>
{
public:
    typedef boost::uuids::uuid value_type;
    typedef boost::uuids::uuid query_type;
    typedef details::buffer image_type;

    static void
    set_value(value_type& uuid, const image_type& b, std::size_t n, bool is_null)
    {
        if (!is_null) {
            std::string tmp(b.data(), n);
            uuid = boost::lexical_cast<value_type>(tmp);
        } else {
            std::memset(uuid.data, 0, 16);
        }

        // called multiple times per load. why?
        // std::cout << "set_value: " << uuid << " is null: " << is_null << " size: " << n << '\n';
    }

    static void
    set_image(image_type& b, std::size_t& n, bool& is_null, const value_type& uuid)
    {
        // std::cout << "set_image" << '\n';
        is_null = false;
        std::string tmp = boost::uuids::to_string(uuid);
        n = tmp.size();

        if (b.capacity() < n) {
            b.capacity(n);
        }
        std::memcpy(b.data(), tmp.data(), n);
    }
};


} /* sqlite */

} /* odb */


#endif /* end of include guard: SEMPR_TRAITS_SQLITE_UUID_HXX */
