#include <sempr/entity/example/CoffeeMug.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
using namespace sempr::entity;



#include <sempr/query/ObjectQuery.hpp>
using namespace sempr::query;

#include <fstream>
#include <iostream>

#include <odb/database.hxx>
#include <Person_odb.h>

#include <sempr/entity/spatial/Point.hpp>
#include <Point_odb.h>

#include <sempr/entity/spatial/Polygon.hpp>
#include <Polygon_odb.h>

#include <sempr/entity/spatial/GeometryCollection.hpp>
#include <RuleSet_odb.h>

#include <sempr/core/IncrementalIDGeneration.hpp>

//#include <cpl_conv.h> // CPLFree for kml export of geometries

#include <sempr/entity/spatial/reference/LocalCS.hpp>
#include <chrono>

#include <RDFDocument_odb.h>

#include <array>

#include <sempr/processing/DebugModule.hpp>
#include <sempr/processing/DBUpdateModule.hpp>
#include <sempr/processing/ActiveObjectStore.hpp>
#include <sempr/processing/SopranoModule.hpp>
#include <sempr/processing/SpatialIndex.hpp>
using namespace sempr::processing;


#include <sempr/sempr.hpp>
using namespace sempr::storage;


#ifndef M_PI
#   define M_PI 3.141592653589793
#endif
/*
void print(OGRGeometry* p)
{
    char* str;
    p->exportToWkt(&str, wkbVariantIso);
    std::cout << str << '\n';
    CPLFree(str);
}
*/

/*
void setupQuadrangle(OGRPolygon* poly, const std::array<float, 3>& min, const std::array<float, 3>& max)
{
    // OGRLineString* ls = (OGRLineString*) OGRGeometryFactory::createGeometry(wkbLineString);
    OGRLinearRing* lr = (OGRLinearRing*) OGRGeometryFactory::createGeometry(wkbLinearRing);
    lr->addPoint(min[0], min[1], max[2]);
    lr->addPoint(min[0], max[1], min[2]);
    lr->addPoint(min[0], max[1], max[2]);
    lr->addPoint(min[0], min[1], min[2]);
    lr->addPoint(min[0], min[1], max[2]);
    lr->addPoint(min[0], max[1], min[2]);
    lr->addPoint(min[0], max[1], max[2]);
    lr->addPoint(max[0], min[1], min[2]);
    lr->addPoint(max[0], min[1], max[2]);
    lr->addPoint(max[0], max[1], min[2]);
    lr->addPoint(max[0], max[1], max[2]);
    lr->closeRings();
    poly->addRingDirectly(lr);
}
*/


int main(int argc, char** args)
{
    /* ************* *
     * SETUP
     * ************** */
    // ODBStorage::Ptr storage( new ODBStorage(":memory:") );
    ODBStorage::Ptr storage( new ODBStorage() );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );

    DebugModule::Ptr debug(new DebugModule());
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );
    SopranoModule::Ptr semantic( new SopranoModule() );
    SpatialIndex::Ptr spatial( new SpatialIndex() );

    sempr::core::IDGenerator::getInstance().setStrategy(
        std::unique_ptr<sempr::core::IncrementalIDGeneration>( new sempr::core::IncrementalIDGeneration(storage) )
    );

    sempr::core::Core c;
    c.addModule(active);
    c.addModule(debug);
    c.addModule(updater);
    c.addModule(semantic);
    c.addModule(spatial);


    /* ************* *
     * TESTS
     * ************** */
     // add a spatial refernce
     LocalCS::Ptr cs(new LocalCS());
     c.addEntity(cs);

     /*
     // add a few geometries
     for (float i = 0.1; i < 10; i++)
     {
         Polygon::Ptr p( new Polygon() );
         setupQuadrangle(p->geometry(), {{i, 0, 0}}, {{i+1, 1, 1}});
         p->setCS(cs);
         c.addEntity(p);
     }
*/

     // query for everything in a box.
     Eigen::Vector3d lower{-0.1, -0.1, -0.1};
     Eigen::Vector3d upper{5.2, 1.2, 1.2};
     auto q = SpatialIndexQuery::withinBox(lower, upper, cs);
     c.answerQuery(q);
     for (auto r : q->results) {
         std::cout << "SpatialIndexQuery result: " << r->id() << '\n';
     }

     std::cout << "and inverted?" << '\n';
     q->invert();
     q->results.clear();
     c.answerQuery(q);
     for (auto r : q->results) {
         std::cout << "SpatialIndexQuery result: " << r->id() << '\n';
     }
}
