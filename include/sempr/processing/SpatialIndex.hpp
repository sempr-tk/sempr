#ifndef SEMPR_PROCESSING_SPATIALINDEX_HPP_
#define SEMPR_PROCESSING_SPATIALINDEX_HPP_

#include <sempr/processing/Module.hpp>
#include <sempr/entity/spatial/Geometry.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <sempr/query/SpatialIndexQuery.hpp>

#include <Geometry_odb.h>   // required for EntityEvent<Geometry>
#include <SpatialReference_odb.h>   // required for EntityEvent<SpatialReference>

#include <vector>
#include <map>

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
//template< std::size_t Dim = 3 >
class SpatialIndex : 
    public Module<  core::EntityEvent<entity::Geometry>,
                     core::EntityEvent<entity::SpatialReference>,
                     query::SpatialIndexQuery<3> >,
    SpatialIndexBase<3>
{
public:
    using Ptr = std::shared_ptr<SpatialIndex>;
    std::string type() const override;

    SpatialIndex(entity::SpatialReference::Ptr rootCS);

    /**
        Answer a SpatialIndexQuery
    */
    void process(query::SpatialIndexQuery<3>::Ptr query) override;


    //const std::map<entity::Geometry::Ptr, bValue>& getGeoBoxes() const;

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
    void process(core::EntityEvent<entity::Geometry>::Ptr geoEvent) override;
    void process(core::EntityEvent<entity::SpatialReference>::Ptr refEvent) override;
    void processChangedCS(entity::SpatialReference::Ptr cs);

    // the actual processing:
    void insertGeo(entity::Geometry::Ptr geo);
    void updateGeo(entity::Geometry::Ptr geo);
    void removeGeo(entity::Geometry::Ptr geo);

    /** Create a pair of bounding-box and ptr */
    ValuePair createEntry(entity::Geometry::Ptr geo, bool query = false) const;    //could throw an exception if there is no tranformation to the rootCS
    entity::Geometry::Ptr findEntry(const ValuePair value) const;


};


}}

#endif /* end of include guard SEMPR_PROCESSING_SPATIALINDEX_HPP_ */
