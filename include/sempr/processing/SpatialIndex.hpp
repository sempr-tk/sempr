#ifndef SEMPR_PROCESSING_SPATIALINDEX_HPP_
#define SEMPR_PROCESSING_SPATIALINDEX_HPP_

#include <sempr/processing/Module.hpp>
#include <sempr/entity/spatial/Geometry.hpp>
#include <sempr/entity/spatial/filter/LocalTransformationFilter.hpp>

#include <geos/geom/Coordinate.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <sempr/query/SpatialIndexQuery.hpp>

#include <Geometry_odb.h>   // required for EntityEvent<Geometry>
#include <SpatialReference_odb.h>   // required for EntityEvent<SpatialReference>

#include <vector>
#include <map>
#include <iterator>

namespace sempr { namespace processing {

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

template <class SpatialEntity>
class SpatialConclusion;


/**
    This class implements a spatial index for geometry-entities. It listens to events of Geometry
    and SpatialReference and answers SpatialQueries, which allows us to ask for geometries which
    do (not) intersect / contain / are within a given axis aligned bounding box referenced to the
    root frame.
    TODO: Regarding the root-frame: Currently the SpatialIndex assumes that everything is located in
    a common root-frame and does neither account for separate chains of LocalCS nor for ProjectionCS
    and GeographicCS being linked by an implicit "earth". We should extend the SpatialIndex *and*
    the SpatialQuery to be able to specifiy which frame we are interested in, and the index should
    only contain geometries which are located relative to the given frame (or a child-frame of it).
    The bridge between ProjectionCS and GeographicCS could be a bit more difficult as we will need
    coordinate transformations done by GDAL in the process.
*/
/**
 * Spatial Index for one specific root coordinate system.
 * All entities that could not be tranformes to this coordinate system will be skipped!
 * Note that the SpatialIndex currently only support 3D Indexing. 2D Geometries will be projected to the zero x/y plane.
 */
template< std::size_t dim = 3 >
class SpatialIndex : 
    public Module<  core::EntityEvent<entity::Geometry>,
                     core::EntityEvent<entity::SpatialReference>,
                     query::SpatialIndexQuery<dim> >,
    SpatialIndexBase<dim>
{
public:
    typedef typename processing::SpatialIndexBase<dim>::Box Box;
    typedef typename processing::SpatialIndexBase<dim>::Point Point;
    typedef typename processing::SpatialIndexBase<dim>::ValuePair ValuePair;
    typedef typename SpatialIndexBase<dim>::RTree RTree;

    using Ptr = std::shared_ptr< SpatialIndex<dim> >;
    std::string type() const override { return "SpatialIndex" + std::to_string(dim) + "D"; }

    SpatialIndex(entity::SpatialReference::Ptr rootCS) :
        rootCS_(rootCS)
    {}

    /**
        Answer a SpatialIndexQuery<dim>
    */
    void process(std::shared_ptr< query::SpatialIndexQuery<dim> > query) override
    {
        std::vector<ValuePair> tmpResults;

        try
        {
            // ToDo: Transform Box and Geom of the Pair into the root ref system.

            Box region = query->refBoxGeometryPair().first;

            typedef query::SpatialIndexQuery<3>::QueryType QueryType;
            switch (query->mode()) {

                case QueryType::WITHIN:
                    rtree_.query(bgi::within(region), std::back_inserter(tmpResults));
                    break;
                case QueryType::NOTWITHIN:
                    rtree_.query(!bgi::within(region), std::back_inserter(tmpResults));
                    break;

                // TODO: contains is introduced in boost 1.55, but ros indigo needs 1.54.
                // maybe its time for me to upgrade to 16.04 and kinetic...
                case QueryType::CONTAINS:
                    rtree_.query(bgi::contains(region), std::back_inserter(tmpResults));
                    break;
                case QueryType::NOTCONTAINS:
                    rtree_.query(!bgi::contains(region), std::back_inserter(tmpResults));
                    break;

                case QueryType::INTERSECTS:
                    rtree_.query(bgi::intersects(region), std::back_inserter(tmpResults));
                    break;
                case QueryType::NOTINTERSECTS:
                    rtree_.query(!bgi::intersects(region), std::back_inserter(tmpResults));
                    break;
                
                default:
                    std::cout << "SpatialIndex: Mode " << query->mode() << " not implemented." << '\n';
            }

        }
        catch (const TransformException& ex)
        {
            //ref geom of the query is in a different cs. Not results.
        }

        for (auto result : tmpResults)
        {
            query->results.insert(findEntry(result));
        }

    }


    const std::map<entity::Geometry::Ptr, ValuePair>& getGeoBoxes() const { return geo2box_; }

private:
    template <class SpatialEntity>
    friend class SpatialConclusion;

    /**
     *   The actual R-Tree.
    */
    RTree rtree_;

    /**
     *  The root reference system for this spatial index and R-Tree
    */
    entity::SpatialReference::Ptr rootCS_;

    /**
     *   A mapping of Geometry-->bValue for easier updates of the RTree
    */
    std::map<entity::Geometry::Ptr, ValuePair> geo2box_;


    /**
     *   Process changes of geometries: New are inserted into the RTree, changed are recomputed
    */
    void process(core::EntityEvent<entity::Geometry>::Ptr geoEvent) override
    {
        typedef core::EntityEvent<entity::Geometry>::EventType EType;
        entity::Geometry::Ptr geo = geoEvent->getEntity();

        switch (geoEvent->what()) 
        {
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

    void process(core::EntityEvent<entity::SpatialReference>::Ptr refEvent) override
    {
        typedef core::EntityEvent<entity::SpatialReference>::EventType EType;

        switch (refEvent->what()) 
        {
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

    void processChangedCS(entity::SpatialReference::Ptr cs)
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

    // the actual processing:
    void insertGeo(entity::Geometry::Ptr geo)
    {
        // sanity: it needs a geometry, and a spatial reference.
        if (!geo->getGeometry() || !geo->getCS()) return;

        // skip geometry with a wrong coordinate dimension
        if (geo->getGeometry()->getCoordinateDimension() != dim) 
        {
            std::cout << "2D Geometry in 3D Index! -- skips this!" << std::endl;
            return;
        }

        try
        {
            // create a new entry
            ValuePair entry = createEntry(geo);
            // save it in our map
            geo2box_[geo] = entry;
            // and insert it into the RTree
            rtree_.insert(entry);
        }
        catch (const TransformException& ex)
        {
            // this will skip the geometry if there is no transformation to the root cs of the spatial index
        }
    }

    void updateGeo(entity::Geometry::Ptr geo)
    {
        // update? lets remove the old one first.
        removeGeo(geo);

        // sanity: it needs a geometry, and a spatial reference.
        if (!geo->getGeometry() || !geo->getCS()) return;

        // and re-insert it with updated data.
        insertGeo(geo);
    }

    void removeGeo(entity::Geometry::Ptr geo)
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

    /** Create a pair of bounding-box and ptr */
    ValuePair createEntry(entity::Geometry::Ptr geo, bool query = false) const //could throw an exception if there is no tranformation to the rootCS
    {
        // get the 3D envelope of the geometry.
        geos::geom::Coordinate geoMin, geoMax;
        geo->findEnvelope(geoMin, geoMax);
/*
        if (dim == 3) && ( (geoMin.z != geoMin.z) || (geoMax.z != geoMax.z) )
            throw TransformException("2D Geometry in " + type() + " but marked as " + std::to_string(dim) + "D coordinate."); //throw exception otherwise boost rtree will throw a confusion one!
            */
        // this envelope is in the coordinate system of the geometry. But what we need is an envelope
        // that is axis aligned with the root reference system. We could transform the geometry to root,
        // take the envelope and transform it back, but that is just ridiculus. Instead: Create a
        // bounding-box-geometry (8 points, one for each corner), transform it, and take its envelope.
        //
        // Note z-coordinate by default is NaN in 2D (defined by geos::geom)
        // ---
        // create a geometry with the matching extends: 8 point, every corner must be checked!
        
        std::vector<geos::geom::Coordinate> cornerCoordinates;

        if (dim == 2)
        {
            geos::geom::Coordinate coord;
            coord = geos::geom::Coordinate(geoMin.x, geoMin.y); cornerCoordinates.push_back(coord);
            coord = geos::geom::Coordinate(geoMin.x, geoMax.y); cornerCoordinates.push_back(coord);
            coord = geos::geom::Coordinate(geoMax.x, geoMin.y); cornerCoordinates.push_back(coord);
            coord = geos::geom::Coordinate(geoMax.x, geoMax.y); cornerCoordinates.push_back(coord);
        }

        if (dim == 3)
        {
            geos::geom::Coordinate coord;
            coord = geos::geom::Coordinate(geoMin.x, geoMin.y, geoMin.z); cornerCoordinates.push_back(coord);
            coord = geos::geom::Coordinate(geoMin.x, geoMin.y, geoMax.z); cornerCoordinates.push_back(coord);
            coord = geos::geom::Coordinate(geoMin.x, geoMax.y, geoMin.z); cornerCoordinates.push_back(coord);
            coord = geos::geom::Coordinate(geoMin.x, geoMax.y, geoMax.z); cornerCoordinates.push_back(coord);
            coord = geos::geom::Coordinate(geoMax.x, geoMin.y, geoMin.z); cornerCoordinates.push_back(coord);
            coord = geos::geom::Coordinate(geoMax.x, geoMin.y, geoMax.z); cornerCoordinates.push_back(coord);
            coord = geos::geom::Coordinate(geoMax.x, geoMax.y, geoMin.z); cornerCoordinates.push_back(coord);
            coord = geos::geom::Coordinate(geoMax.x, geoMax.y, geoMax.z); cornerCoordinates.push_back(coord);
        }

        entity::MultiPoint::Ptr mpEntity( new entity::MultiPoint() );    // Note: this wast IDs - recommended to use a factory in future

        mpEntity->setCoordinates(cornerCoordinates);
        mpEntity->setCS(geo->getCS());

        // transfrom it to the reference system of the R-Tree
        mpEntity->transformToCS(rootCS_);   //could throw an exception if there is no transformation from geo cs to root cs

        /*  // really strange happenings - this is different to the manually apply of the env filter!!!
        geos::geom::Coordinate mpMin, mpMax;
        geo->findEnvelope(mpMin, mpMax);

        // create the bBox out of bPoints.
        bBox box(
            bPoint(mpMin.x, mpMin.y, mpMin.z),
            bPoint(mpMax.x, mpMax.y, mpMax.z)
        );

        */

        EnvelopeFilter ef;
        mpEntity->getGeometry()->apply_ro(ef);

        // create the bBox out of bPoints.
        Box box;

        if (dim == 2)
        {
            box = Box(
                Point(ef.getMin().x, ef.getMin().y),
                Point(ef.getMax().x, ef.getMax().y)
            );
        }
        if (dim == 3)
        {
            box = Box(
                Point(ef.getMin().x, ef.getMin().y, ef.getMin().z),
                Point(ef.getMax().x, ef.getMax().y, ef.getMax().z)
            );
        }

        // create a transformed copy of the geometry.
        auto geom = geo->clone();
        geom->transformToCS(rootCS_);
        
        return ValuePair(box, geom);
    }
    
    
    entity::Geometry::Ptr findEntry(const ValuePair value) const
    {
        for (auto it = geo2box_.begin(); it != geo2box_.end(); ++it)
        {
            if (it->second.second == value.second)
                return it->first;
        }

        return nullptr;
    }

};

typedef SpatialIndex<2> SpatialIndex2D;
typedef SpatialIndex<3> SpatialIndex3D;

}}

#endif /* end of include guard SEMPR_PROCESSING_SPATIALINDEX_HPP_ */
