#ifndef SEMPR_PROCESSING_SPATIAL_RESTRICTOR_HPP_
#define SEMPR_PROCESSING_SPATIAL_RESTRICTOR_HPP_

#include <geos/geom/Geometry.h>

#include <sempr/processing/Module.hpp>
#include <sempr/processing/SpatialIndex.hpp>
#include <sempr/query/SpatialRestrictionQuery.hpp>

#include <sempr/entity/spatial/Geometry.hpp>
#include <sempr/entity/spatial/reference/GeodeticCS.hpp>

#include <sempr/entity/RDFVector.hpp>

#include <GeometricObject_odb.h>
#include <Geometry_odb.h>           // required for EntityEvent<Geometry>
#include <SpatialReference_odb.h>   // required for EntityEvent<SpatialReference>

#include <set>
#include <map>

namespace sempr { namespace processing {

class RestrictionBuilder
{
public:
    using Ptr = std::shared_ptr<RestrictionBuilder>;

    virtual entity::Geometry::Ptr operator()(const entity::Geometry::Ptr geo) const = 0;
};

// The SpatialEntity have to implement a geometry() method to get a geometry entity pointer and a type() method to get the type information as string.
/**
 * @brief The first Version of the SpatialRestrictor. This Version will trigger on events of SpatialEntity. The SpatialEntity have to implement a geometry() and type() method.
 * The restrictor creates spatial restricting geometry based on the type of the SpatialEntities.
 * 
 * @todo:   In the next version the restrictor shall only trigger on RDF triples which could be build up by a reasoner or a SPARQL query. 
 *          For this the RDFStore (Soprano) should do their processing before this unit before and this processing unit shall only be triggered by a specfic rdf pattern.
 * 
 * @tparam SpatialEntity A template parameter that could be placed somewhere in the inheritacne hierarchy to select a strict type (e.g. a Tree class or a SpatialObject class only if it represents the type "tree" )
 */
template < EXTENDS_CLASS(SpatialEntity, entity::AbstractSpatialObject) >
class SpatialRestrictor : public Module< core::EntityEvent<SpatialEntity>, core::EntityEvent<entity::SpatialReference>, query::SpatialRestrictionQuery >
{
public:
    using Ptr = std::shared_ptr< SpatialRestrictor<SpatialEntity> >;

    // Returns the type of this processing unit
    std::string type() const override { return "SpatialRestrictor"; }

    typedef std::vector<std::string> StringList;

    // The RestrictionTruple groups a list of types where the spatial striction is build on, the restriction geometry builder and a list of restriction rdf objects (the restrictor will build the URIs of this).
    typedef std::tuple<StringList, const RestrictionBuilder::Ptr, StringList> RestrictionTuple;


    /**
     * Create a SpatialRestrictor
     * 
     * @param restrictionObjectType Type name of the restricting objects. (no URI)
     */
    SpatialRestrictor(const std::string& restrictionObjectType = "RestrictionArea") :
        restrictionObjectType_(sempr::buildURI(restrictionObjectType , sempr::baseURI()))
    {
    };

    void process(std::shared_ptr< core::EntityEvent<SpatialEntity> > event) override
    {
        typedef core::EntityEventBase::EventType EventType;

        switch(event->what()) {
            case EventType::CREATED:
            case EventType::LOADED:
                addEntity(event->getEntity());
                break;
            case EventType::REMOVED:
                removeEntity(event->getEntity());
                break;
            case EventType::CHANGED:
                updateEntity(event->getEntity());
                break;
            default:
            // nothing
                break;
        }

    }

    void process(core::EntityEvent<entity::SpatialReference>::Ptr refEvent) override
    {
        typedef core::EntityEvent<entity::SpatialReference>::EventType EType; //will ony be called if the spatial reference itself is changed

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

    void process(std::shared_ptr< query::SpatialRestrictionQuery > query) override
    {
        //ToDo
    }

    /**
     * Register a new restriction.
     * 
     * The truple shall look like:
     *  0 : StringList for the relevant types of the restriction. (empty means for all)
     *  1 : RestrictionBuilder
     *  2 : Restriction object as rdf object value of this restriction. 
     * 
     * The predicate is used to build up the rdf truple of the restricting geometries.
     */
    void registerRestriction(const RestrictionTuple& truple, std::string predicate = "restriction")
    {
        restrictionTuples_.push_back(std::make_pair(truple, predicate));
    }


    std::vector<entity::GeometricObject::Ptr> getRestrictions() const
    {
        std::vector<entity::GeometricObject::Ptr> results;

        for (auto resPair : restrictionMap_)
        {
            results.insert(std::end(results), std::begin(resPair.second), std::end(resPair.second));
        }

        return results;
    }

    std::vector<entity::GeometricObject::Ptr> getRestrictions(const std::shared_ptr<SpatialEntity>& spatialEntity) const
    {
        try
        {
            return restrictionMap_.at(spatialEntity);
        } 
        catch (const std::exception& ex)
        { 
            // nothing to do. Only catch the exception and return an empty vector.
        }
        return std::vector<entity::GeometricObject::Ptr>();
    }


private:
    const std::string restrictionObjectType_;

    std::vector< std::pair<RestrictionTuple, std::string> > restrictionTuples_;

    // maps a SpatialEntity to restriction geometries
    std::map<const std::shared_ptr<SpatialEntity>, std::vector<entity::GeometricObject::Ptr> > restrictionMap_;
    
    bool removeEntity(const std::shared_ptr<SpatialEntity>& spatialEntity, bool notify = true)
    {
        if (restrictionMap_.find(spatialEntity) != restrictionMap_.end())
        {
            auto geometries = restrictionMap_.at(spatialEntity);

            //notify removed geometry in any case:
            for (auto geom : geometries)
            {
                geom->geometry()->removed();
            }

            if (notify)
            {
                for (auto geom : geometries)
                {
                    geom->removed();
                }
            }
        }

        return restrictionMap_.erase(spatialEntity);
    }

    bool addEntity(const std::shared_ptr<SpatialEntity>& spatialEntity, bool notify = true)
    {
        bool added = false;

        for ( auto tuplePair : restrictionTuples_ )
        {
            if ( checkType( spatialEntity, std::get<0>(tuplePair.first) ) )
            {
                auto restri = buildRestrictionGeo(spatialEntity, std::get<1>(tuplePair.first), std::get<2>(tuplePair.first), tuplePair.second);

                if (restri)
                {
                    restrictionMap_[spatialEntity].push_back(restri);

                    // notify the new geometry in any case:
                    restri->geometry()->created();

                    if (notify)
                        restri->created();

                    added = true;
                }
            }
        }

        return added;
    }

    void updateEntity(const std::shared_ptr<SpatialEntity>& spatialEntity)
    {
        removeEntity(spatialEntity);
        addEntity(spatialEntity);
    }

    void processChangedCS(entity::SpatialReference::Ptr cs)
    {
        // we need to check every geometry currently in the index, if it is affected.
        for (auto entry : restrictionMap_)
        {
            if (entry.first->geometry()->getCS()->isChildOf(cs))
            {
                updateEntity(entry.first);
            }
        }
    }

    bool checkType(const std::shared_ptr<SpatialEntity>& spatialEntity, const StringList& types) const
    {
        if (types.empty())
            return true;    //empty typelist will passthough all spatial enities

        auto find = std::find(types.begin(), types.end(), spatialEntity->type());

        return find != types.end(); //type of spatial Entity is found in the type list
    }

    entity::GeometricObject::Ptr buildRestrictionGeo(const std::shared_ptr<SpatialEntity>& spatialEntity, const RestrictionBuilder::Ptr restrictionBuilder, const StringList& restrictions, const std::string& predicate) const
    {
        auto geo = (*restrictionBuilder)(spatialEntity->geometry());   // call the restriction builder

        if (geo)
        {
            geo->setCS(spatialEntity->geometry()->getCS()); // set the cs of the generated geometry to the source one.
            geo->created();

            entity::GeometricObject::Ptr geomObject(new entity::GeometricObject(true)); //build a temporary geometric object
            geomObject->geometry(geo);
            geomObject->type(restrictionObjectType_);

            for (auto restriction : restrictions)
            {
                geomObject->registerProperty(sempr::buildURI(predicate , sempr::baseURI()), restriction);
            }

            return geomObject;
        }
        else
        {
            return nullptr;
        }
    }

};

//CentroidCircle
// If the extended radius is 0 than this builder will only create a bounding sphere (not minimal!) of the geometry.
class CentroidCircleBuilder : public RestrictionBuilder
{
public:
    CentroidCircleBuilder( double extendedRadius ) : rExt_(extendedRadius) {}

    virtual entity::Geometry::Ptr operator()(const entity::Geometry::Ptr geo) const override
    {
        auto centroid = *(geo->getGeometry()->getCentroid()->getCoordinate());

        geos::geom::Coordinate min, max;
        geo->findEnvelope(min, max);

        double distanceToMax = centroid.distance(max);
        double distanceToMin = centroid.distance(min);

        double r = distanceToMax > distanceToMin ? distanceToMax : distanceToMin; // set r to max distance

        r += rExt_;  // extend the radius

        return buildPolygonCircle(centroid, r);
    }

private:
    double rExt_;

    entity::Geometry::Ptr buildPolygonCircle(const geos::geom::Coordinate& origin, double r, unsigned short resolution = 16) const
    {
        if (resolution < 4 || resolution % 4 != 0)
            return NULL;

        std::vector<geos::geom::Coordinate> coords;

        double angleBetween = 2 * M_PI / resolution;

        for (double angle = 0; angle < 2 * M_PI; angle += angleBetween)
        {
            double x = r * cos(angle);
            double y = r * sin(angle);

            auto coord = geos::geom::Coordinate(origin.x + x, origin.y + y);

            coords.push_back(coord);
        }

        coords.push_back(coords[0]);    //close ring/polygon

        entity::Polygon::Ptr polygon( new entity::Polygon() );

        polygon->setCoordinates(coords);

        return polygon;
    }

};

/**
 * @brief A RestrictionBuilder based on the GEOS GEOM buffer() method.
 */
class BufferBuilder : public RestrictionBuilder
{
public:
    BufferBuilder(double distance) : distance_(distance) {}

    virtual entity::Geometry::Ptr operator()(const entity::Geometry::Ptr geo) const override
    {
        auto buffered = geo->getGeometry()->buffer(distance_); // set buffered geometry

        return buildGeometry(buffered);
    }

private:
    const double distance_;

    entity::Geometry::Ptr buildGeometry(geos::geom::Geometry* buffered) const
    {
        auto geoType = buffered->getGeometryTypeId();
        switch (geoType)
        {
            case geos::geom::GEOS_POINT:
            {
                entity::Point::Ptr point(new entity::Point());
                point->setGeometry( dynamic_cast<geos::geom::Point*>(buffered) );
                return point;
            }
            case geos::geom::GEOS_LINESTRING:
            {
                entity::LineString::Ptr ls(new entity::LineString());
                ls->setGeometry( dynamic_cast<geos::geom::LineString*>(buffered) );
                return ls;
            }
            case geos::geom::GEOS_LINEARRING:
            {
                entity::LinearRing::Ptr lr(new entity::LinearRing());
                lr->setGeometry( dynamic_cast<geos::geom::LinearRing*>(buffered) );
                return lr;
            }
            case geos::geom::GEOS_POLYGON:
            {
                entity::Polygon::Ptr poly(new entity::Polygon());
                poly->setGeometry( dynamic_cast<geos::geom::Polygon*>(buffered) );
                return poly;
            }
            case geos::geom::GEOS_MULTIPOINT:
            {
                entity::MultiPoint::Ptr mp(new entity::MultiPoint());
                mp->setGeometry( dynamic_cast<geos::geom::MultiPoint*>(buffered) );
                return mp;
            }
            case geos::geom::GEOS_GEOMETRYCOLLECTION:
            case geos::geom::GEOS_MULTIPOLYGON:
            case geos::geom::GEOS_MULTILINESTRING:
            {
                entity::GeometryCollection::Ptr collection(new entity::GeometryCollection());
                collection->setGeometry( dynamic_cast<geos::geom::GeometryCollection*>(buffered) );
                return collection;
            }
        }
        return nullptr;
    }
};

}}

#endif /* end of include guard SEMPR_PROCESSING_SPATIAL_RESTRICTOR_HPP_ */
