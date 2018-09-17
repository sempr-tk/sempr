#ifndef SEMPR_PROCESSING_SPATIAL_CONCLUSION_HPP_
#define SEMPR_PROCESSING_SPATIAL_CONCLUSION_HPP_

#include <sempr/processing/Module.hpp>
#include <sempr/processing/SpatialIndex.hpp>

#include <sempr/entity/spatial/Geometry.hpp>
#include <sempr/entity/spatial/reference/GeodeticCS.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <sempr/entity/RDFVector.hpp>

#include <Geometry_odb.h>           // required for EntityEvent<Geometry>
#include <SpatialReference_odb.h>   // required for EntityEvent<SpatialReference>

#include <set>
#include <map>

namespace sempr { namespace processing {


/**
 * The SpatialConclusion is an extension to the SpatialIndex.
 * It will provide RDF Triple for a set of spatial relations to allow Spatial SPARQL queries.
 * 
 * @prefix geo: <http://www.w3.org/2003/01/geo/wgs84_pos#>
 * @prefix ogc: <http://www.opengis.net/ont/geosparql#>
 * ogc:sfEquals
 * ogc:sfDisjoint   //b
 * ogc:sfIntersects //b
 * ogc:sfTouches    //?
 * ogc:sfWithin     //b
 * ogc:sfContains   //b
 * ogc:sfOverlaps   //b
 * ogc:sfCrosses    //only for multipoint/polygon, multipoint/linestring, linestring/linestring, linestring/polygon, and linestring/multipolygon comparisons.
 * 
 * @prefix spatial: <http://jena.apache.org/spatial#>
 * spatial:north
 * spatial:south
 * spatial:west
 * spatial:east
 * 
 * Note: To fullfill the GeoSPARQL Entity the SpatialEntity have to be makred in RDF as SubClassOf ogc:SpatialObject and the depending geometry as ogc:Geometry
 * 
 * Note: Other JenaSpatial relations like nearby, withinCircle, withinBox and intersectBox are only covered by SpatialIndexQuery
 * 
 * Info: Understanding spatial relations: http://edndoc.esri.com/arcsde/9.1/general_topics/understand_spatial_relations.htm
 * 
 * Future on this will also provide 3D relations like:
 * above
 * below
 * on       (like above but connected!)
 * under    (like below but connected!)
 * 
 * 
 * The SpatialEntity have to implement a geometry() method to get a geometry object pointer.
 * 
 * The ODB header for the SpatialEntity (*_odb.h) have to be included before this module get included.
 */


template <class SpatialEntity>
class SpatialConclusion : public Module< core::EntityEvent<SpatialEntity>, core::EntityEvent<entity::SpatialReference> >
{
public:
    using Ptr = std::shared_ptr< SpatialConclusion<SpatialEntity> >;

    // isGlobal is set if both geometries are transformed in the same global reference system
    typedef std::function<bool(const SpatialIndex::bBox& self,const SpatialIndex::bBox& other, bool isGlobal)> CheckBoxFunction;
    typedef std::function<bool(const entity::Geometry::Ptr& self,const entity::Geometry::Ptr& other, bool isGlobal)> CheckGeometryFunction;

    SpatialConclusion(const std::weak_ptr<SpatialIndex>& spatialIndex) :
        index_(spatialIndex)
    { 
        globalRoot_ = false;
        if (auto idx = index_.lock())
        {
            auto globalTest = std::dynamic_pointer_cast<entity::GlobalCS>(idx->rootCS_);
            if(globalTest)
                globalRoot_ = true;
        }


        initDefaultChecker();
    };

    void process(std::shared_ptr< core::EntityEvent<SpatialEntity> > event) override
    {
        typedef core::EntityEventBase::EventType EventType;

        // assume that the geometry of the spatial entity is already handeld!
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
        // todo found and update the based enities
    }   

    void registerCheckFunction(const std::string& relationPredicate, const CheckBoxFunction& checker)
    {
        checkBoxFunctions_[relationPredicate] = checker;
    }



private:
    std::weak_ptr<SpatialIndex> index_;

    std::map<entity::Geometry::Ptr, std::string> spatialGeometry_;

    std::map<std::string, entity::RDFVector::Ptr> rdfMap_;  // temporary rdf vector for every entity (mapped by id)

    bool globalRoot_;

    std::map<std::string, CheckBoxFunction> checkBoxFunctions_;


    void removeEntity(const std::shared_ptr<SpatialEntity>& entity)
    {
        rdfMap_[entity->id()]->removed();   // fire remove event

        removeGeometryLink(entity->id());

        rdfMap_.erase(entity->id());        // remove rdf vector for the entity
        removeBackRelation(entity->id());   // remove linked back relations ot his entity
    }


    void addEntity(const std::shared_ptr<SpatialEntity>& entity, bool change = false)
    {
        if (indexedGeometry(entity->geometry()))
        {
            if (!change)
            {   // new Entity
                rdfMap_[entity->id()] = entity::RDFVector::Ptr(new entity::RDFVector(true)); // not persistant Entity
                rdfMap_[entity->id()]->created();
            }
            else
            {   // changed Entity
                rdfMap_[entity->id()]->clear();
                rdfMap_[entity->id()]->changed();
            }

            spatialGeometry_[entity->geometry()] = entity->id();    // build up geometry link
            checkEntity(entity->id(), entity->geometry());
        }
        //otherwise skip this entity!

    }

    bool indexedGeometry(const std::shared_ptr<entity::Geometry>& geometry)
    {
        if (auto idx = index_.lock())
        {
            return idx->geo2box_.find(geometry) != idx->geo2box_.end();
        }
    }

    bool removeGeometryLink(const std::string& id)
    {
        // the geometry could changed but the id have to be the same all the time
        for (auto it = spatialGeometry_.begin(); it != spatialGeometry_.end(); ++it)
        {
            if ( it->second == id )
            {
                spatialGeometry_.erase(it);
                return true;
            }
        }
        return false;
    }

    void updateEntity(const std::shared_ptr<SpatialEntity>& entity)
    {
        // There are two alternative way to handle this case:
        // 1. remove the old entity and add the updated one (easy but with many overhat)
        // 2. check that have be changed and check specific for the changes. (not easy but efficient)
        // For now we take the first option

        removeGeometryLink(entity->id());
        removeBackRelation(entity->id());
        addEntity(entity, true);
    }


    void removeBackRelation(const std::string& id)
    {
        std::string objID = sempr::baseURI() + id;

        for (auto rdfIt = rdfMap_.begin(); rdfIt != rdfMap_.end(); ++rdfIt)
        {
            std::vector<entity::Triple> toRemove;

            for (auto tIt = rdfIt->second->begin(); tIt != rdfIt->second->end(); ++tIt)
            {
                // search for object id in back linked rfd triple
                if ((*tIt).object == objID)
                {
                    toRemove.emplace_back(*tIt);
                }
            }

            bool rdfChanged = false;
            for (auto remove : toRemove)
            {
                rdfIt->second->removeTriple(remove);
                rdfChanged = true;
            }

            if (rdfChanged)
                rdfIt->second->changed();

        }


    }


    // Note: will not create a event for the self depending rdf vector
    void checkEntity(const std::string& id, const entity::Geometry::Ptr& self)
    {
        auto idx = index_.lock();
        if (idx)
        {
            auto selfBox = idx->geo2box_[self].first;

            std::set<entity::RDFVector::Ptr> changedRDF;    //set for all changed RDFVectors by this method

            // check every registered check function 
            for (auto checkIt = checkBoxFunctions_.begin(); checkIt != checkBoxFunctions_.end(); ++checkIt)
            {

                for (auto other : idx->rtree_)
                {
                    //check from self to others
                    bool selfRelated = checkIt->second(self, other.first, globalRoot_);
                    if (selfRelated)
                    {
                        // Build Triple: SelfId, Function predicate, OtherID
                        entity::Triple t(sempr::baseURI() + id, checkIt->first, sempr::baseURI() + spatialGeometry_.at(other.second));
                        rdfMap_[id]->addTriple(t, true);
                    }

                    //check from others to self
                    bool otherRelated = checkIt->second(other.first, self, globalRoot_);
                    if (otherRelated)
                    {
                        auto otherID = spatialGeometry_.at(other.second);
                        // Build Triple: OtherID, Function predicate, SelfId
                        entity::Triple t(sempr::baseURI() + otherID, checkIt->first, sempr::baseURI() + id);
                        rdfMap_[otherID]->addTriple(t, true);
                        changedRDF.insert(rdfMap_[otherID]);    //mark vactor as changed
                    }
                }

            }

            // notify changes of rdf vectors from others
            for (auto rdfVector : changedRDF)
                rdfVector->changed();
        }    

    }

    void initDefaultChecker()
    {
        registerCheckFunction("spatial:north", checkNorthOf);
        registerCheckFunction("spatial:south", checkSouthOf);
        registerCheckFunction("spatial:east", checkEastOf);
        registerCheckFunction("spatial:west", checkWestOf);
    }


    static bool check2D(const SpatialIndex::bBox& test)
    {
        double h = abs(test.min_corner().get<2>() - test.max_corner().get<2>());
        return h < 0.00001; // objects with a high less than 0.1mm 
    }

    static bool checkNorthOf(const SpatialIndex::bBox& self, const SpatialIndex::bBox& other, bool isGlobal)
    {
        if (isGlobal)
        {
            return self.min_corner().get<0>() >= other.max_corner().get<0>(); // x coordinate is lat (only in WGS84)
        }
        else
        {
            return false;
        }
    }

    static bool checkSouthOf(const SpatialIndex::bBox& self, const SpatialIndex::bBox& other, bool isGlobal)
    {
        if (isGlobal)
        {
            return self.max_corner().get<0>() <= other.min_corner().get<0>(); // x coordinate is lat (only in WGS84)
        }
        else
        {
            return false;
        }
    }

    static bool checkEastOf(const SpatialIndex::bBox& self, const SpatialIndex::bBox& other, bool isGlobal)
    {
        if (isGlobal)
        {
            return self.min_corner().get<1>() >= other.max_corner().get<1>(); // y coordinate is lon (only in WGS84)
        }
        else
        {
            return false;
        }
    }

    static bool checkWestOf(const SpatialIndex::bBox& self, const SpatialIndex::bBox& other, bool isGlobal)
    {
        if (isGlobal)
        {
            return self.max_corner().get<1>() <= other.min_corner().get<1>(); // y coordinate is lon (only in WGS84)
        }
        else
        {
            return false;
        }
    }



};


}}

#endif /* end of include guard SEMPR_PROCESSING_SPATIAL_CONCLUSION_HPP_ */
