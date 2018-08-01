#include <sempr/entity/spatial/Geometry.hpp>
#include <Geometry_odb.h>

#include <sempr/entity/spatial/reference/GlobalCS.hpp>
#include <sempr/entity/spatial/filter/LocalTransformationFilter.hpp>

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
}

Geometry::~Geometry() 
{
}

const geom::Geometry* Geometry::getGeometry() const
{
    return getGeometryMut();
}

geom::Geometry* Geometry::getGeometryMut() const
{ 
    return nullptr; // have to be override by childs!
}

Geometry::Ptr Geometry::clone() const 
{
    // raw clone is virtual! :)
    return Geometry::Ptr(raw_clone());
}

Geometry* Geometry::raw_clone() const 
{
    return nullptr;
}

void Geometry::setCS(SpatialReference::Ptr cs) 
{
    referenceFrame_ = cs;
}

SpatialReference::Ptr Geometry::getCS() const 
{
    return referenceFrame_;
}

void Geometry::apply(Filter& filter)
{
    geom::Geometry* geometry = getGeometryMut();   // best match call to the protected getter. If this is non-const you can only call the public getter with a const return value.

    if (!geometry)
        throw TransformException("no geometry to apply a filter!");

    geometry->apply_rw(filter);
}

void Geometry::apply(FilterList& filterList)
{
    for (auto filter : filterList)
    {
        apply(*filter);
    }
}


void Geometry::transformToCS(SpatialReference::Ptr cs) 
{
    auto geometry = getGeometry();

    if (!geometry)
        throw TransformException("no geometry to transform");

    if (!referenceFrame_)
        throw TransformException("source reference frame invalid");

    if (!cs)
        throw TransformException("target reference frame invalid");


    auto rootThis = referenceFrame_->getRoot();
    auto rootOther = cs->getRoot();

    // two cases to consider:
    // 1. both geometries are in the same "tree", have the same root
    // (e.g. UTM 32N, or a common LocalCS)
    if (rootThis == rootOther) 
    {
        // same root reference frame, can focus on LocalCS only
        auto fromThis = referenceFrame_->transformationToRoot();
        auto toOther = cs->transformationFromRoot();
        // geometry = toOther * fromThis * geometry
        LocalTransformationFilter tf(toOther * fromThis);
        apply(tf);
    }

    // 2. the geometries have different roots but both a global (e.g., one is on WGS84,
    //    and the other in a local coordinate system relative to UTM 32N)
    else 
    {
        GlobalCS::Ptr globalSrc = std::dynamic_pointer_cast<GlobalCS>(rootThis);
        GlobalCS::Ptr globalDst = std::dynamic_pointer_cast<GlobalCS>(rootOther);

        if(globalSrc && globalDst)
        {
            // both are global!
            auto globalTransform = globalSrc->to(globalDst);
            if (globalTransform.empty()) {
                // transformation unknown
                throw TransformException("global transform unknown");
            }
            // 3 steps:

            // 1: this from ref to this->getRoot()
            auto thisToRoot = referenceFrame_->transformationToRoot();
            LocalTransformationFilter tfToRoot(thisToRoot);
            apply(tfToRoot);

            // 2: from this->getRoot() to cs->getRoot()
            apply(globalTransform);

            // 3: from cs->getRoot() to cs
            auto rootToCS = cs->transformationFromRoot();
            LocalTransformationFilter tfToCS(rootToCS);
            apply(tfToCS);
        }
        else
        {
            // we have two different roots that are not both of type
            // GlobalCS? no way to transform between them.
            std::cout << globalSrc.get() << '\n';
            std::cout << globalDst.get() << '\n';
            throw TransformException("different root systems that are not global");
        }

    }

    // set new reference frame
    referenceFrame_ = cs;
}

void Geometry::findEnvelope(geom::Coordinate& min, geom::Coordinate& max)
{
    EnvelopeFilter envFilter;
    apply(envFilter);   //apply the filter to find the min and max values of this geometry

    min = envFilter.getMin();
    max = envFilter.getMax();
}


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

    auto dim = geom->getCoordinateDimension(); //why isnt this done in the WKTWriter?
    wktWriter.setOutputDimension(dim);

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
