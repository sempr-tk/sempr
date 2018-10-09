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
 * ogc:sfIntersects
 * ogc:sfWithin
 * ogc:sfContains
 * ogc:sfOverlaps
 * 
 * @prefix spatial: <http://jena.apache.org/spatial#>
 * spatial:north
 * spatial:south
 * spatial:west
 * spatial:east
 * 
 * Note: To fullfill the GeoSPARQL the SpatialEntity have to be marked in RDF as SubClassOf ogc:SpatialObject and the depending geometry as ogc:Geometry
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
 * perpendicular
 *
 * Relations in discussion:
 * ogc:sfEquals
 * ogc:sfTouches
 * ogc:sfDisjoint   // negation of intersects
 * ogc:sfCrosses    // only for multipoint/polygon, multipoint/linestring, linestring/linestring, linestring/polygon, and linestring/multipolygon comparisons
 * 
 * The SpatialEntity have to implement a geometry() method to get a geometry object pointer.
 * 
 * The ODB header for the SpatialEntity (*_odb.h) have to be included before this module get included.
 */


template < std::size_t dim, class SpatialEntity>
class SpatialConclusion : public Module< core::EntityEvent<SpatialEntity>, core::EntityEvent<entity::SpatialReference> >
{
public:
    using Ptr = std::shared_ptr< SpatialConclusion<dim, SpatialEntity> >;

    typedef typename processing::SpatialIndexBase<dim>::ValuePair ValuePair;
    typedef typename processing::SpatialIndexBase<dim>::Box Box;

    // isGlobal is set if both geometries are transformed in the same global reference system
    //typedef std::function<bool(const SpatialIndex<dim>::Box& self,const SpatialIndex<dim>::Box& other, bool isGlobal)> CheckBoxFunction;
    typedef std::function<bool(const ValuePair& self, const ValuePair& other, const entity::SpatialReference::Ptr& ref)> CheckFunction;

    SpatialConclusion(const std::weak_ptr< SpatialIndex<dim> >& spatialIndex) :
        index_(spatialIndex)
    { 
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

    // register a new check function for a given predicate like north or east.
    // The check functions could assume that the geometries are already converted in the same coordinate system!
    bool registerCheckFunction(const std::string& relationPredicate, const CheckFunction& checker)
    {
        if (checkURI(relationPredicate))
        {
            checkFunctions_[relationPredicate] = checker;
            return true;
        } else
        {
            return false;
        }
    }

    entity::RDFVector::Ptr getConclusion(const std::shared_ptr<SpatialEntity>& entity)
    {
        return rdfMap_.at(entity->id());
    }


private:
    std::weak_ptr< SpatialIndex<dim> > index_;

    std::map<entity::Geometry::Ptr, std::string> spatialGeometry_;

    std::map<std::string, entity::RDFVector::Ptr> rdfMap_;  // temporary rdf vector for every entity (mapped by id)

    std::map<std::string, CheckFunction> checkFunctions_;


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

        return false;
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
        std::string objURI = sempr::buildURI(id);

        for (auto rdfIt = rdfMap_.begin(); rdfIt != rdfMap_.end(); ++rdfIt)
        {
            std::vector<entity::Triple> toRemove;

            for (auto tIt = rdfIt->second->begin(); tIt != rdfIt->second->end(); ++tIt)
            {
                // search for object id in back linked rfd triple
                if ((*tIt).object == objURI)
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
            auto selfPair = idx->geo2box_[self];

            std::set<entity::RDFVector::Ptr> changedRDF;    //set for all changed RDFVectors by this method

            // check every registered box check function 
            for (auto checkBoxIt = checkFunctions_.begin(); checkBoxIt != checkFunctions_.end(); ++checkBoxIt)
            {

                for (auto other : idx->geo2box_)
                {
                    if (selfPair.second == other.second.second)
                        continue;   //skip conclusion if self == other!

                    //check from self to others
                    bool selfRelated = checkBoxIt->second(selfPair, other.second, index_.lock()->rootCS_);
                    if (selfRelated)
                    {
                        // Build Triple: SelfId, Function predicate, OtherID
                        entity::Triple t(   sempr::buildURI(id),
                                            checkBoxIt->first,
                                            sempr::buildURI(spatialGeometry_.at(other.first)) );
                        rdfMap_[id]->addTriple(t, true);
                    }

                    //check from others to self
                    bool otherRelated = checkBoxIt->second(other.second, selfPair, index_.lock()->rootCS_);
                    if (otherRelated)
                    {
                        auto otherID = spatialGeometry_.at(other.first);
                        // Build Triple: OtherID, Function predicate, SelfId
                        entity::Triple t(   sempr::buildURI(otherID),
                                            checkBoxIt->first,
                                            sempr::buildURI(id)     );
                        rdfMap_[otherID]->addTriple(t, true);
                        changedRDF.insert(rdfMap_[otherID]);    //mark vector as changed
                    }
                }

            }

            // notify changes of rdf vectors from others
            for (auto rdfVector : changedRDF)
                rdfVector->changed();
        }    

    }

    bool checkURI(const std::string& uri)
    {
        bool isURI = true;

        try {
            isURI = isURI && uri.at(0) == '<';
            isURI = isURI && uri.at(uri.length() - 1) == '>';
            isURI = isURI && uri.find(':') != std::string::npos;
        } catch (const std::out_of_range& oor) {
            return false;
        }

        return isURI;
    }

    bool checkShortcut(const std::string& uri)
    {
        bool isShortcut = true;
        try {
            isShortcut = isShortcut && uri.at(0) != '<';
            isShortcut = isShortcut && uri.at(uri.length() - 1) != '>';
            isShortcut = isShortcut && uri.find(':') != std::string::npos; // a shortcut need a ':' between prefix and id.
            isShortcut = isShortcut && uri.find('#') == std::string::npos; // a shortcut will not contain a hash symbol.
        } catch (const std::out_of_range& oor) {
            return false;
        }

        return isShortcut;
    }

    // register the default set of check functions
    void initDefaultChecker()
    {
        registerCheckFunction("<http://jena.apache.org/spatial#north>", directionCheck<entity::GlobalCS::NORTH>);
        registerCheckFunction("<http://jena.apache.org/spatial#east>",  directionCheck<entity::GlobalCS::EAST>);
        registerCheckFunction("<http://jena.apache.org/spatial#south>", directionCheck<entity::GlobalCS::SOUTH>);
        registerCheckFunction("<http://jena.apache.org/spatial#west>",  directionCheck<entity::GlobalCS::WEST>);

        registerCheckFunction("<http://www.opengis.net/ont/geosparql#sfIntersects>",  intersectionCheck);
        registerCheckFunction("<http://www.opengis.net/ont/geosparql#sfWithin>",      withinCheck);
        registerCheckFunction("<http://www.opengis.net/ont/geosparql#sfContains>",    containsCheck);
        registerCheckFunction("<http://www.opengis.net/ont/geosparql#sfOverlaps>",    overlapsCheck);

    }

    // A generic direction check function for north east south west relations. The direction is equal to the GlobalCS direction enum. 
    // The test assume that both pairs are in the same global spatial reference system.
    template <entity::GlobalCS::CardinalDirection direction>
    static bool directionCheck(const ValuePair& self, const ValuePair& other, const entity::SpatialReference::Ptr& ref)
    {
        auto globalRef = std::dynamic_pointer_cast<entity::GlobalCS>(ref);

        if (globalRef)
        {
            const std::size_t axis = globalRef->directionDimension(direction);
            bool positive = direction <= 1; // North and East are the positive directions

            auto selfBox = self.first;
            auto otherBox = other.first;

            if (positive)
            {
                auto selfMin = getMin(selfBox, axis);
                auto otherMax = getMax(otherBox, axis);
                return selfMin >= otherMax;
            }
            else
            {
                auto selfMax = getMax(selfBox, axis);
                auto otherMin = getMin(otherBox, axis);
                return selfMax <= otherMin;
            }
        }

        return false;
    }

    static bool intersectionCheck(const ValuePair& self, const ValuePair& other, const entity::SpatialReference::Ptr& ref)
    {
        if (boost::geometry::intersects(self.first, other.first))
        {
            if (self.second && other.second)
            {
                return self.second->getGeometry()->intersects(other.second->getGeometry());
            }
            return true;
        }
        return false;
    }

    static bool withinCheck(const ValuePair& self, const ValuePair& other, const entity::SpatialReference::Ptr& ref)
    {
        if (boost::geometry::within(self.first, other.first))
        {
            if (self.second && other.second)
            {
                return self.second->getGeometry()->within(other.second->getGeometry());
            }
            return true;
        }
        return false;
    }

    static bool containsCheck(const ValuePair& self, const ValuePair& other, const entity::SpatialReference::Ptr& ref)
    {
        if (boost::geometry::within(self.first, other.first))
        {
            if (self.second && other.second)
            {
                return self.second->getGeometry()->contains(other.second->getGeometry());
            }
            return true;
        }
        return false;
    }

    static bool overlapsCheck(const ValuePair& self, const ValuePair& other, const entity::SpatialReference::Ptr& ref)
    {
        if (boost::geometry::overlaps(self.first, other.first))
        {
            if (self.second && other.second)
            {
                return self.second->getGeometry()->overlaps(other.second->getGeometry());
            }
            return true;
        }
        return false;
    }


    static double getMin(const Box& box, const std::size_t axis)
    {
        //return bg::get<axis>(box.min_corner()); //constexpr. error!
        return processing::SpatialIndexBase<dim>::toEigen(box.min_corner())[axis];
    }

    static double getMax(const Box& box, const std::size_t axis)
    {
        //return bg::get<axis>(box.max_corner()); //constexpr. error
        return processing::SpatialIndexBase<dim>::toEigen(box.max_corner())[axis];
    }


};

template <class SpatialEntity>
using SpatialConclusion2D = SpatialConclusion<2, SpatialEntity>;

template <class SpatialEntity>
using SpatialConclusion3D = SpatialConclusion<3, SpatialEntity>;


}}

#endif /* end of include guard SEMPR_PROCESSING_SPATIAL_CONCLUSION_HPP_ */
