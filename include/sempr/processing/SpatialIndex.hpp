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
class SpatialIndex
    : public Module< core::EntityEvent<entity::Geometry>,
                     core::EntityEvent<entity::SpatialReference>,
                     query::SpatialIndexQuery >
{
public:
    using Ptr = std::shared_ptr<SpatialIndex>;
    std::string type() const override;

    SpatialIndex();

    /**
        Answer a SpatialIndexQuery
    */
    void process(query::SpatialIndexQuery::Ptr query) override;

    /**
        Specify what is stored in the R-Tree:
            boxes, made out of points, consisting of 3 floats, in cartesian space.
        NOTE: Boost seems to support geographic and spherical coordinates (lat-long etc) here, how
        does this affect the RTree? Can we use this to support indexing on lat-lon later on?
    */
    typedef bg::model::point<float, 3, bg::cs::cartesian> bPoint;
    typedef bg::model::box<bPoint> bBox;
    typedef std::pair<bBox, entity::Geometry::Ptr> bValue;
    typedef bgi::rtree<bValue, bgi::quadratic<16> > RTree;

private:
    /**
        The actual R-Tree.
    */
    RTree rtree_;

    /**
        A mapping of Geometry-->bValue for easier updates of the RTree
    */
    std::map<entity::Geometry::Ptr, bValue> geo2box_;


    /**
        Process changes of geometries: New are inserted into the RTree, changed are recomputed
    */
    void process(core::EntityEvent<entity::Geometry>::Ptr geoEvent) override;
    void process(core::EntityEvent<entity::SpatialReference>::Ptr refEvent) override;
    void processChangedCS(entity::SpatialReference::Ptr cs);

    // the actual processing:
    void insertGeo(entity::Geometry::Ptr geo);
    void updateGeo(entity::Geometry::Ptr geo);
    void removeGeo(entity::Geometry::Ptr geo);

    /** Create a pair of bounding-box and ptr */
    bValue createEntry(entity::Geometry::Ptr geo);
};


}}

#endif /* end of include guard SEMPR_PROCESSING_SPATIALINDEX_HPP_ */
