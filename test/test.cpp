#define BOOST_TEST_MODULE sempr-test
#include <boost/test/included/unit_test.hpp>

#include <sempr/sempr.hpp>
using namespace sempr::core;
using namespace sempr::storage;
using namespace sempr::processing;

BOOST_AUTO_TEST_CASE(first_test)
{
    ODBStorage::Ptr storage( new ODBStorage() );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );
    
    Core core(storage);
    core.addModule(active);
    core.addModule(updater);
    
    // TODO: write real test cases.
    
}
