#ifndef SEMPR_PROCESSING_SPATIAL_RESTRICTOR_HPP_
#define SEMPR_PROCESSING_SPATIAL_RESTRICTOR_HPP_

#include <geos/geom/Geometry.h>

#include <sempr/processing/Module.hpp>
#include <sempr/processing/SpatialIndex.hpp>


#include <sempr/entity/spatial/Geometry.hpp>
#include <sempr/entity/spatial/reference/GeodeticCS.hpp>

#include <sempr/entity/RDFVector.hpp>

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

template <class SpatialEntity>
class SpatialRestrictor : public Module< core::EntityEvent<SpatialEntity>, core::EntityEvent<entity::SpatialReference> >
{
public:
    using Ptr = std::shared_ptr< SpatialRestrictor<SpatialEntity> >;

    // Returns the type of this processing unit
    std::string type() const override { return "SpatialRestrictor"; }

    typedef std::vector<std::string> StringList;

    // The RestrictionTruple groups a list of types where the spatial striction is build on, the restriction geometry builder and a list of restriction rdf objects (the restrictor will build the URIs of this).
    typedef std::tuple<StringList, const RestrictionBuilder::Ptr, StringList> RestrictionTruple;


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
    void registerRestriction(const RestrictionTruple& truple, std::string predicate = "restriction")
    {
        restrictionTruples_.push_back(std::make_pair(truple, predicate));
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

    std::vector< std::pair<RestrictionTruple, std::string> > restrictionTruples_;

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

        for ( auto truplePair : restrictionTruples_ )
        {
            if ( checkType( spatialEntity, std::get<0>(truplePair.first) ) )
            {
                auto restri = buildRestrictionGeo(spatialEntity, std::get<1>(truplePair.first), std::get<2>(truplePair.first), truplePair.second);

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

//ExtendedHullBuilder
// If the distance is 0 this builder will pass through the convex hull of the geometry.
class ExtendedHullBuilder : public RestrictionBuilder
{
    virtual entity::Geometry::Ptr operator()(const entity::Geometry::Ptr geo) const override
    {
        return nullptr; //ToDo
    }
};

}}

#endif /* end of include guard SEMPR_PROCESSING_SPATIAL_RESTRICTOR_HPP_ */
