#include <sempr/processing/SpatialIndex.hpp>
#include <sempr/core/EntityEvent.hpp>
#include <sempr/entity/spatial/LocalTransformation.hpp>
#include <iterator>

#include <gdal/ogr_core.h>

namespace sempr { namespace processing {

SpatialIndex::SpatialIndex()
{
    // listen to geometries
    this->addOverload<core::EntityEvent<entity::Geometry> >(
        [this](core::EntityEvent<entity::Geometry>::Ptr event) {
            this->process(event);
        }
    );

    // listen to spatial references
    this->addOverload<core::EntityEvent<entity::SpatialReference> >(
        [this](core::EntityEvent<entity::SpatialReference>::Ptr event) {
            this->process(event);
        }
    );

    // listen to spatial queries
    this->addOverload<query::SpatialIndexQuery>(
        [this](query::SpatialIndexQuery::Ptr query) {
            this->lookup(query);
        }
    );
}


std::string SpatialIndex::type() const
{
    return "SpatialIndex";
}

void SpatialIndex::lookup(query::SpatialIndexQuery::Ptr query) const
{
    // TODO: transfer reference geometry into the frame of the spatial index
    // (make a copy?)
    // query->refGeo_

    // create the AABB of the transformed query-volume.
    OGREnvelope3D env;
    query->refGeo()->geometry()->getEnvelope(&env);

    bBox region(
        bPoint(env.MinX, env.MinY, env.MinZ),
        bPoint(env.MaxX, env.MaxY, env.MaxZ)
    );


    std::vector<bValue> tmpResults;

    typedef query::SpatialIndexQuery::QueryType QueryType;
    switch (query->mode()) {
        case QueryType::WITHIN:
            rtree_.query(bgi::within(region), std::back_inserter(tmpResults));
            break;

        default:
            std::cout << "SpatialIndex: Mode " << query->mode() << " not implemented." << '\n';
    }

    std::transform( tmpResults.begin(), tmpResults.end(),
                    std::inserter(query->results, query->results.end()),
         [](bValue tmp) { return tmp.second; }
    );
}


void SpatialIndex::process(core::EntityEvent<entity::Geometry>::Ptr geoEvent)
{
    typedef core::EntityEvent<entity::Geometry>::EventType EType;
    entity::Geometry::Ptr geo = geoEvent->getEntity();

    switch (geoEvent->what()) {
        case EType::CREATED:
        case EType::LOADED:
            insertGeo(geo);
            break;
        case EType::CHANGED:
            updateGeo(geo);
            break;
        case EType::REMOVED:
            removeGeo(geo);
            break;
    }
}

void SpatialIndex::process(core::EntityEvent<entity::SpatialReference>::Ptr refEvent)
{
    typedef core::EntityEvent<entity::SpatialReference>::EventType EType;
    switch (refEvent->what()) {
        case EType::CREATED:
        case EType::LOADED:
            // nothing to do.
            break;
        case EType::CHANGED:
            // check which geometries are affected and update them.
            processChangedCS(refEvent->getEntity());
            break;
        case EType::REMOVED:
            // well...
            // what happens if we remove a SpatialReference that some geometry is pointing to?
            // (why would be do that? how can we prevent that?)
            break;
    }
}


void SpatialIndex::processChangedCS(entity::SpatialReference::Ptr cs)
{
    // we need to check every geometry currently in the index, if it is affected.
    for (auto entry : geo2box_)
    {
        if (entry.first->getCS()->isChildOf(cs))
        {
            // well, in that case update it.
            updateGeo(entry.first);
        }
    }
}


SpatialIndex::bValue SpatialIndex::createEntry(entity::Geometry::Ptr geo)
{
    // get the 3d envelope of the geometry.
    OGREnvelope3D envelope;
    geo->geometry()->getEnvelope(&envelope);
    // this envelope is in the coordinate system of the geometry. But what we need is an envelope
    // that is axis aligned with the root reference system. We could transform the geometry to root,
    // take the envelope and transform it back, but that is just ridiculus. Instead: Create a
    // bounding-box-geometry (8 points, one for each corner), transform it, and take its envelope.
    // ---
    // create a geometry with the matching extends: 8 point, every corner must be checked!
    OGRMultiPoint mp;
    OGRPoint* p;
    p = new OGRPoint(envelope.MinX, envelope.MinY, envelope.MinZ); mp.addGeometryDirectly(p);
    p = new OGRPoint(envelope.MinX, envelope.MinY, envelope.MaxZ); mp.addGeometryDirectly(p);
    p = new OGRPoint(envelope.MinX, envelope.MaxY, envelope.MinZ); mp.addGeometryDirectly(p);
    p = new OGRPoint(envelope.MinX, envelope.MaxY, envelope.MaxZ); mp.addGeometryDirectly(p);
    p = new OGRPoint(envelope.MaxX, envelope.MinY, envelope.MinZ); mp.addGeometryDirectly(p);
    p = new OGRPoint(envelope.MaxX, envelope.MinY, envelope.MaxZ); mp.addGeometryDirectly(p);
    p = new OGRPoint(envelope.MaxX, envelope.MaxY, envelope.MinZ); mp.addGeometryDirectly(p);
    p = new OGRPoint(envelope.MaxX, envelope.MaxY, envelope.MaxZ); mp.addGeometryDirectly(p);

    // transform the geometry
    LocalTransformation tf(geo->getCS()->transformationToRoot());
    mp.transform(&tf);

    // get the new envelope
    mp.getEnvelope(&envelope);
    // mp will run out of scope and destroy the points with it.

    // create the bBox out of bPoints.
    bBox box(
        bPoint(envelope.MinX, envelope.MinY, envelope.MinZ),
        bPoint(envelope.MaxX, envelope.MaxY, envelope.MaxZ)
    );

    return bValue(box, geo);
}


void SpatialIndex::insertGeo(entity::Geometry::Ptr geo)
{
    // sanity: it needs a geometry, and a spatial reference.
    if (!geo->geometry() || !geo->getCS()) return;

    // create a new entry
    bValue entry = createEntry(geo);
    // save it in our map
    geo2box_[geo] = entry;
    // and insert it into the RTree
    rtree_.insert(entry);


    std::cout << "inserted " << geo->id() << " into spatial index" << '\n';
}

void SpatialIndex::updateGeo(entity::Geometry::Ptr geo)
{
    // update? lets remove the old one first.
    removeGeo(geo);

    // sanity: it needs a geometry, and a spatial reference.
    if (!geo->geometry() || !geo->getCS()) return

    // and re-insert it with updated data.
    insertGeo(geo);
}

void SpatialIndex::removeGeo(entity::Geometry::Ptr geo)
{
    // find the map-entry
    auto it = geo2box_.find(geo);
    if (it != geo2box_.end())
    {
        // remove from rtree
        rtree_.remove(it->second);
        // and from the map
        geo2box_.erase(it);
    }
}

}}
