#ifndef SEMPR_TRAITS_SQLITE_SPATIAL_REFERENCE_HXX
#define SEMPR_TRAITS_SQLITE_SPATIAL_REFERENCE_HXX

#include <odb/sqlite/traits.hxx>
#include <ogr_spatialref.h>
#include <cpl_conv.h>

#include <cstddef> // std::size_t
#include <cstring> // std::strncmp, std::memcpy
#include <sstream>
#include <iostream>

#include <type_traits>

namespace odb { namespace sqlite {

/**
    Methods to persist an OGRSpatialReference in an sqlite-database.
*/
template <>
class value_traits<OGRSpatialReference, id_text>
{
public:
    typedef OGRSpatialReference value_type;
    typedef OGRSpatialReference query_type;
    typedef details::buffer image_type;

    // spatial ref from text
    static void set_value(value_type& spatialref, const image_type& b, std::size_t n, bool is_null)
    {
        if (!is_null) {
            char* ptr = const_cast<char*>(b.data());
            spatialref.importFromWkt(&ptr);
        } else {
            // well... what to to with a spatialreference that is null in the database. wont happen.
        }
    }

    // image from spatialref (--> save into database)
    static void set_image(image_type& b, std::size_t& n, bool& is_null, const value_type& spatialref)
    {
        is_null = false;
        char* wkt;
        spatialref.exportToWkt(&wkt);
        {
            std::string tmp(wkt);
            n = tmp.size();

            if (b.capacity() < n) {
                b.capacity(n);
            }
            std::memcpy(b.data(), tmp.data(), n);
        }
        CPLFree(wkt);
    }
};




} /* sqlite */

} /* odb */


#endif /* end of include guard: SEMPR_TRAITS_SQLITE_SPATIAL_REFERENCE_HXX */
