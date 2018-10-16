#include "test_utils.hpp"
using namespace testing;






BOOST_AUTO_TEST_SUITE(spatial_restriction)
    std::string dbfile = "test_spatial_restriction_sqlite.db";

    BOOST_AUTO_TEST_CASE(spatial_restriction_basics)
    {
        Core core;
        
        SpatialRestrictor2D<GeometricObject>::Ptr spatialRes = SpatialRestrictor2D<GeometricObject>::Ptr( new SpatialRestrictor2D<GeometricObject>( {}, std::make_shared<CentroidCircle>(3.), {} ) );

    }

    BOOST_AUTO_TEST_CASE(spatial_conclusion_cleanup)
    {
        removeStorage(dbfile);
    }

BOOST_AUTO_TEST_SUITE_END()
