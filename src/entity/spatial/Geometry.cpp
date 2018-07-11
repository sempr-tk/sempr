#include <sempr/entity/spatial/Geometry.hpp>
#include <sempr/entity/spatial/GlobalCS.hpp>
#include <sempr/entity/spatial/LocalTransformation.hpp>
#include <Geometry_odb.h>

#include <geos/geom/GeometryFactory.h>
#include <geos/io/WKBReader.h>
#include <geos/io/WKBWriter.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>


namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(Geometry)

const geom::GeometryFactory* Geometry::factory_ = geom::GeometryFactory::getDefaultInstance();

Geometry::Geometry() : Geometry(new core::IDGen<Geometry>())
{
}

Geometry::Geometry(const core::IDGenBase* idgen)
    : Entity(idgen)
{
    this->setDiscriminator<Geometry>();

    //factory geometry creation by sub classes!
}

Geometry::~Geometry() 
{
    //factory geometry delete by sub classes!
}

geom::Geometry* Geometry::geometry() 
{ 
    return geometry_;
}

void Geometry::setGeometry(geom::Geometry* geometry) 
{ 
    if (geometry != NULL)
    {
        factory_->destroyGeometry(geometry_);
        geometry_ = geometry;
    }
}

Geometry::Ptr Geometry::clone() const 
{
    // raw clone is virtual! :)
    return Geometry::Ptr(raw_clone());
}

Geometry* Geometry::raw_clone() const 
{
    return NULL;
}

void Geometry::setCS(SpatialReference::Ptr cs) 
{
    referenceFrame_ = cs;
}

SpatialReference::Ptr Geometry::getCS() const 
{
    return referenceFrame_;
}

/*
void Geometry::transformToCS(SpatialReference::Ptr cs) {
    if (!this->geometry())
        throw TransformException("no geometry to transform");

    if (!this->referenceFrame_)
        throw TransformException("source reference frame invalid");

    if (!cs)
        throw TransformException("target reference frame invalid");


    auto rootThis = referenceFrame_->getRoot();
    auto rootOther = cs->getRoot();

    // two cases to consider:
    // 1. both geometries are in the same "tree", have the same root
    // (e.g. UTM 32N, or a common LocalCS)
    if (rootThis == rootOther) {
        // same root reference frame, can focus on LocalCS only
        auto fromThis = referenceFrame_->transformationToRoot();
        auto toOther = cs->transformationFromRoot();
        // geometry = toOther * fromThis * geometry
        LocalTransformation tf(toOther * fromThis);
        this->geometry()->transform(&tf);
    }

    // 2. the geometries have different roots (e.g., one is on WGS84,
    //    and the other in a local coordinate system relative to UTM 32N)
    else {
        GlobalCS::Ptr globalThis = std::dynamic_pointer_cast<GlobalCS>(rootThis);
        GlobalCS::Ptr globalOther = std::dynamic_pointer_cast<GlobalCS>(rootOther);

        if (!globalThis || !globalOther) {
            // we have two different roots that are not both of type
            // GlobalCS? no way to transform between them.
            std::cout << globalThis.get() << '\n';
            std::cout << globalOther.get() << '\n';
            throw TransformException("different root systems that are not global");
        }

        // both are global!
        auto transform = globalThis->to(globalOther);
        if (!transform) {
            // transformation unknown to GDAL / proj4?!
            throw TransformException("transform unknown to GDAL/proj4");
        }
        // 3 steps:
        // 1: this from ref to this->getRoot()
        auto thisToRoot = referenceFrame_->transformationToRoot();
        LocalTransformation tfToRoot(thisToRoot);
        this->geometry()->transform(&tfToRoot);
        // 2: from this->getRoot() to cs->getRoot()
        this->geometry()->transform(transform.get());
        // 3: from cs->getRoot() to cs
        auto rootToCS = cs->transformationFromRoot();
        LocalTransformation tfToCS(rootToCS);
        this->geometry()->transform(&tfToCS);
    }

    // set new reference frame
    referenceFrame_ = cs;
}
*/

geom::Geometry* Geometry::importFromWKB(const std::basic_string<char>& buffer)
{
    const geom::GeometryFactory& factory = *geom::GeometryFactory::getDefaultInstance();

    geos::io::WKBReader wkbReader(factory);

    std::istringstream iss(buffer);

    geom::Geometry* geom = NULL;
    try {
        geom = wkbReader.read(iss);
    }
    catch (const std::exception& e) {
        //todo handle the exception
    }

    return geom;
}

std::basic_string<char> Geometry::exportToWKB(const geom::Geometry* geom)
{
    auto wkbWriter = geos::io::WKBWriter();

    std::basic_ostringstream<char> oss;

    try {
        wkbWriter.write(*geom, oss);
    }
    catch (const std::exception& e) {
        //todo handle the exception
    }

    return oss.str();
}

geom::Geometry* Geometry::importFromWKT(const std::string& text)
{
    const geom::GeometryFactory& factory = *geom::GeometryFactory::getDefaultInstance();

    geos::io::WKTReader wktReader(factory);

    geom::Geometry* geom = NULL;
    try {
        geom = wktReader.read(text);
    }
    catch (const std::exception& e) {
        //todo handle the exception
    }

    return geom;
}

std::string Geometry::exportToWKT(const geom::Geometry* geom)
{
    auto wktWriter = geos::io::WKTWriter();

    std::string wkt;

    try {
        wkt = wktWriter.write(geom);
    }
    catch (const std::exception& e) {
        //todo handle the exception
    }

    return wkt;
}



}}
