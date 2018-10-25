#ifndef SEMPR_UTILS_H
#define SEMPR_UTILS_H

#include <boost/test/unit_test.hpp>

#include <odb/core.hxx>
#include <sempr/sempr.hpp>
#include <sempr/entity/example/Person.hpp>
#include <Person_odb.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include <sempr/entity/RDFPropertyMap.hpp>

#include <sempr/query/ObjectQuery.hpp>
#include <sempr/query/SPARQLQuery.hpp>
#include <sempr/core/IncrementalIDGeneration.hpp>

// geometries
#include <Geometry_odb.h>
#include <Collection_odb.h>
#include <Lineal_odb.h>
#include <GeometryCollection_odb.h>
#include <Point_odb.h>
#include <LineString_odb.h>
#include <LinearRing_odb.h>
#include <Polygon_odb.h>
#include <MultiPoint_odb.h>

#include <PointCloud_odb.h>


#include <sempr/entity/spatial/SpatialObject.hpp>
#include <SpatialObject_odb.h>
#include <GeometricObject_odb.h>

// reference systems
#include <sempr/entity/spatial/reference/LocalCS.hpp>
#include <sempr/entity/spatial/reference/GeodeticCS.hpp>
#include <sempr/entity/spatial/reference/GeocentricCS.hpp>
#include <sempr/entity/spatial/reference/ProjectionCS.hpp>

#include <sempr/entity/spatial/reference/UniversalTransverseMercatorCS.hpp>
#include <sempr/entity/spatial/reference/UniversalPolarStereographicCS.hpp>
#include <sempr/entity/spatial/reference/LocalTangentPlaneCS.hpp>
#include <sempr/entity/spatial/reference/MilitaryGridReferenceSystem.hpp>

#include <sempr/processing/SopranoModule.hpp>
#include <sempr/processing/SpatialIndex.hpp>
#include <sempr/processing/SpatialConclusion.hpp>

using namespace sempr::core;
using namespace sempr::storage;
using namespace sempr::processing;
using namespace sempr::entity;
using namespace sempr::query;

namespace testing {

  #define DISCRIMINATOR(T) (odb::object_traits_impl<T, odb::id_common>::info.discriminator)

  inline std::string toString(const geom::Geometry* p)
  {
      return Geometry::exportToWKT(p);
  }

  inline void print(const geom::Geometry* p)
  {
      std::cout << toString(p) << '\n';
  }

  inline ODBStorage::Ptr setUpStorage(const std::string& db_path, bool reset){

    BOOST_CHECK(!boost::filesystem::exists(db_path) );
    ODBStorage::Ptr storage( new ODBStorage(db_path, reset) );
    // set the strategy to IncrementalIDGeneration.
    IDGenerator::getInstance().setStrategy(
        std::unique_ptr<IDGenerationStrategy>(
            new IncrementalIDGeneration(storage)
        )
    );

    BOOST_CHECK(boost::filesystem::exists(db_path) );

    return storage;
  }

  inline ODBStorage::Ptr loadStorage(const std::string& db_path){

    BOOST_CHECK(boost::filesystem::exists(db_path));

    // need to reset the IDGeneration first: It keeps a shared_ptr to storage,
    // and storage keeps a session. If we didn't do this, we'd get
    // "std::exception: session already in effect in this thread"
    IDGenerator::getInstance().reset();

    ODBStorage::Ptr storage( new ODBStorage(db_path, false) );
    // set the strategy to IncrementalIDGeneration.
    IDGenerator::getInstance().setStrategy(
        std::unique_ptr<IDGenerationStrategy>(
            new IncrementalIDGeneration(storage)
        )
    );

    return storage;
  }

  inline void removeStorage(const std::string& db_path){
    IDGenerator::getInstance().reset(); // free the current strategy
    // (which keeps a pointer to ODBStorage)

    boost::filesystem::remove(db_path);
    BOOST_CHECK(!boost::filesystem::exists(db_path) );
  }

  inline void checkEntitiesInStorage(ODBStorage::Ptr storage, int n){
    std::vector<DBObject::Ptr> all;
    storage->loadAll(all);
    BOOST_CHECK_EQUAL(all.size(), n);
  }

  template <class Entity>
  inline void checkEntitiesInStorage(ODBStorage::Ptr storage, int n)
  {
      std::vector<std::shared_ptr<Entity> > all;
      storage->loadAll<Entity>(all);
      BOOST_CHECK_EQUAL(all.size(), n);
  }
}

#endif
