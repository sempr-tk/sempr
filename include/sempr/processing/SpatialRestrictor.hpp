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

template < std::size_t dim, class SpatialEntity>
class SpatialRestrictor : public Module< core::EntityEvent<SpatialEntity>, core::EntityEvent<entity::SpatialReference> >
{
public:
    using Ptr = std::shared_ptr< SpatialRestrictor<dim, SpatialEntity> >;

    typedef std::vector<std::string> StringList;

    SpatialRestrictor(const StringList& types, const RestrictionBuilder::Ptr restrictionBuilder, const StringList restrictions) :
        types_(types),
        restrictionBuilder_(restrictionBuilder),
        restrictions_(restrictions)
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
                //processChangedCS(refEvent->getEntity());
                break;
            case EType::REMOVED:
                // well...
                // what happens if we remove a SpatialReference that some geometry is pointing to?
                // (why would be do that? how can we prevent that?)
                break;
        }
    }   

private:
    StringList types_;
    const RestrictionBuilder::Ptr restrictionBuilder_;
    StringList restrictions_;

    // Maps the ID of the SpatialEntity to a restriction
    std::map<const std::shared_ptr<SpatialEntity>, entity::GeometricObject::Ptr> restrictionMap_;


    bool removeEntity(const std::shared_ptr<SpatialEntity>& spatialEntity, bool notify = true)
    {
        if (restrictionMap_.find(spatialEntity) != restrictionMap_.end())
        {
            //notify removed geometry in any case:
            restrictionMap_.at(spatialEntity)->geometry()->removed();

            if (notify)
                restrictionMap_.at(spatialEntity)->removed();
        }

        return restrictionMap_.erase(spatialEntity);
    }

    bool addEntity(const std::shared_ptr<SpatialEntity>& spatialEntity, bool notify = true)
    {
        if (checkType(spatialEntity))
        {
            auto restri = buildRestrictionGeo(spatialEntity);
            if (restri)
            {
                restrictionMap_[spatialEntity] = restri;

                // notify the new geometry in any case:
                restri->geometry()->created();

                if (notify)
                    restri->created();

                return true;
            }
        }
        return false;
    }

    void updateEntity(const std::shared_ptr<SpatialEntity>& spatialEntity)
    {
        removeEntity(spatialEntity, false);
        addEntity(spatialEntity, false);

        if (restrictionMap_.find(spatialEntity) != restrictionMap_.end())
            restrictionMap_.at(spatialEntity)->changed();
    }

    void processChangedCS(entity::SpatialReference::Ptr cs)
    {
        // we need to check every geometry currently in the index, if it is affected.
        for (auto entry : restrictionMap_)
        {
            if (entry.first->geometry()->getCS()->isChildOf(cs))
            {
                // well, in that case update it.
                updateEntity(entry.first);
            }
        }
    }

    bool checkType(const std::shared_ptr<SpatialEntity>& spatialEntity)
    {
        if (types_.empty())
            return true;    //empty typelist will passthough all spatial enities

        auto find = std::find(types_.begin(), types_.end(), spatialEntity->type());

        return find != types_.end(); //type of spatial Entity is found in the type list
    }

    entity::GeometricObject::Ptr buildRestrictionGeo(const std::shared_ptr<SpatialEntity>& spatialEntity)
    {
        auto geo = (*restrictionBuilder_)(spatialEntity->geometry());   // call the restriction builder

        if (geo)
        {
            geo->setCS(spatialEntity->geometry()->getCS()); // set the cs of the generated geometry to the source one.

            entity::GeometricObject::Ptr geomObject(new entity::GeometricObject(true)); //build a temporary geometric object
            geomObject->geometry(geo);
            geomObject->type(sempr::buildURI("RestrictionArea" , sempr::baseURI()));

            for (auto restriction : restrictions_)
            {
                geomObject->registerProperty(sempr::buildURI("restriction" , sempr::baseURI()), restriction);
            }

            return geomObject;
        }
        else
        {
            return nullptr;
        }
    }

};


template <class SpatialEntity>
using SpatialRestrictor2D = SpatialRestrictor<2, SpatialEntity>;

template <class SpatialEntity>
using SpatialRestrictor3D = SpatialRestrictor<3, SpatialEntity>;



//CentroidCircle
class CentroidCircle : public RestrictionBuilder
{
public:
    CentroidCircle( double extendedRadius ) : rExt_(extendedRadius) {}

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

//ExtendedHull


}}

#endif /* end of include guard SEMPR_PROCESSING_SPATIAL_RESTRICTOR_HPP_ */
