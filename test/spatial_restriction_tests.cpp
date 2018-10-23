#include "test_utils.hpp"
using namespace testing;


BOOST_AUTO_TEST_SUITE(spatial_restriction)
    std::string dbfile = "test_spatial_restriction_sqlite.db";

    BOOST_AUTO_TEST_CASE(spatial_restriction_basics)
    {
        Core core;
        
        SpatialRestrictor<GeometricObject>::Ptr spatialRes( new SpatialRestrictor<GeometricObject>() );

        auto restrictionTuple = SpatialRestrictor<GeometricObject>::RestrictionTruple({}, BufferBuilder(3), {});

        spatialRes->registerRestriction(restrictionTuple);

    }

    BOOST_AUTO_TEST_CASE(spatial_conclusion_cleanup)
    {
        removeStorage(dbfile);
    }

BOOST_AUTO_TEST_SUITE_END()
