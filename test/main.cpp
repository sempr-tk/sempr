#include <sempr/sempr.hpp>
using namespace sempr::storage;

#include <sempr/entity/CoffeeMug.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
using namespace sempr::entity;

#include <sempr/processing/DebugModule.hpp>
#include <sempr/processing/DBUpdateModule.hpp>
#include <sempr/processing/ActiveObjectStore.hpp>
using namespace sempr::processing;

#include <sempr/query/ObjectQuery.hpp>
using namespace sempr::query;

#include <fstream>
#include <iostream>

#include <odb/database.hxx>
#include <Person_odb.h>

#include <sempr/entity/spatial/Point.hpp>
#include <Point_odb.h>

#include <sempr/core/IncrementalIDGeneration.hpp>

#include <gdal/cpl_conv.h> // CPLFree for kml export of geometries

int main(int argc, char** args)
{
    // ODBStorage::Ptr storage( new ODBStorage(":memory:") );
    ODBStorage::Ptr storage( new ODBStorage() );
    DebugModule::Ptr debug( new DebugModule() );
    DBUpdateModule::Ptr updater( new DBUpdateModule(storage) );
    ActiveObjectStore::Ptr active( new ActiveObjectStore() );

    sempr::core::IDGenerator::getInstance().setStrategy(
        // std::unique_ptr<sempr::core::UUIDGeneration>( new sempr::core::UUIDGeneration(false) )
        std::unique_ptr<sempr::core::IncrementalIDGeneration>( new sempr::core::IncrementalIDGeneration(storage) )
    );

    sempr::core::Core c(storage);
    c.addModule(active);
    c.addModule(debug);
    c.addModule(updater);

    for (int i = 0; i < 7; i++)
    {
        // create and insert a point
        Point::Ptr p(new Point());
        p->geometry()->setCoordinateDimension(3);
        p->geometry()->setX(1);
        p->geometry()->setY(2);
        p->geometry()->setZ(3);
        p->geometry()->setMeasured(true);
        p->geometry()->setM(1.23);
        c.addEntity(p);
    }

    {
        // load all entities
        std::vector<Entity::Ptr> entities;
        storage->loadAll(entities);
        for (auto e : entities) e->loaded();

        // query for points
        ObjectQuery<Point>::Ptr query(new ObjectQuery<Point>());
        c.answerQuery(query);
        std::cout << "found " << query->results.size() << " points." << '\n';
        for (auto r : query->results) {
            char* str;
            r->geometry()->exportToWkt(&str, wkbVariantIso);
            std::cout << "point: " << str << '\n';
            CPLFree(str);
        }
    }




    return 0;
}
