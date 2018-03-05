#include <sempr/sempr.hpp>
using namespace sempr::storage;

#include <sempr/entity/CoffeeMug.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
using namespace sempr::entity;

#include <sempr/processing/DebugModule.hpp>
#include <sempr/processing/DBUpdateModule.hpp>
#include <sempr/processing/ActiveObjectStore.hpp>
#include <sempr/processing/SopranoModule.hpp>
using namespace sempr::processing;

#include <sempr/query/ObjectQuery.hpp>
using namespace sempr::query;

#include <fstream>
#include <iostream>

#include <odb/database.hxx>
#include <Person_odb.h>

#include <sempr/entity/spatial/Point.hpp>
#include <Point_odb.h>

#include <sempr/entity/spatial/GeometryCollection.hpp>
#include <RuleSet_odb.h>

#include <sempr/core/IncrementalIDGeneration.hpp>

#include <cpl_conv.h> // CPLFree for kml export of geometries

#include <sempr/entity/spatial/LocalCS.hpp>

#include <RDFDocument_odb.h>
#include <Polygon_odb.h>

#ifndef M_PI
#   define M_PI 3.141592653589793
#endif


#include <sempr/utility/Wrapper.hpp>

class FooBar {
public:
    FooBar() { std::cout << "Ctor of FooBar!" << '\n'; }
};

int main(int argc, char** args)
{
    std::cout << "Wrapper<Polygon>" << '\n';
    Wrapper<Polygon> w1;
    std::cout << "Wrapper<Polygon, FooBar>" << '\n';
    Wrapper<Polygon, FooBar> w1foobar;
    std::cout << "Wrapper<RDFPropertyMap>" << '\n';
    Wrapper<RDFPropertyMap> w2;
}
