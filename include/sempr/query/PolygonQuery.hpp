#ifndef SEMPR_QUERY_POLYGONQUERY_HPP_
#define SEMPR_QUERY_POLYGONQUERY_HPP_

#include <sempr/query/Query.hpp>
#include <sempr/query/ObjectQuery.hpp>
#include <sempr/storage/DBObject.hpp>
#include <odb/core.hxx>
#include <memory>

#include <sempr/entity/agro/AgroEntity.hpp>
#include <sempr/entity/spatial/PointCloud_2.hpp>
#include <sempr/entity/spatial/Polygon.hpp>
#include <sempr/entity/spatial/MultiPoint.hpp>

#include <PointCloud_2_odb.h>

#include <geos/geom/Coordinate.h>



//namespace geom = geos::geom;

namespace sempr { namespace query {

    /**
        Object-Queries intend to return a list of objects that match some given
        criteria. This base class allows us to accept and answer type-based
        queries by using a string to discriminate between different derived
        classes of DBObject. This sounds evil and hacky, but in fact is not: It
        relies on the discriminator used and auto-assigned by the odb-compiler!
        (See the DBObject.typeid - database-table column.)
        The discriminator of a given class can be retrieved through the
        expression
            'odb::object_traits_imp<MyClass, odb::id_common>::info.discriminator'
        whereas the actual discriminator of an object instance is accessible
        through
            'obj->discriminator()'.
        TODO: The latter is a bit hacky: We created a view (DBObject_type) that
        maps a DBObject to its typeid-field, as it seems that it cannot be read
        in another way. Be aware that a 'View' in odb is _not_ a database-view,
        but a (pre-compiled?) sql-query.
    */
    class PolygonQuery : public Query, public core::OType<PolygonQuery> {
    public:
        using Ptr = std::shared_ptr<PolygonQuery>;

        PolygonQuery(const geom::Polygon *poly);

        PolygonQuery(entity::agro::AgroEntity::Ptr &agro);
        PolygonQuery(entity::Polygon::Ptr &polygon);

        ~PolygonQuery();
        std::string type() const override { return "PolygonQuery"; }

        /** the vector of points, that are within the polygon of this entity **/
        std::vector <geom::Coordinate> results;

        void calculatePoints(const entity::PointCloud2::Ptr cloud);

    private:

        PolygonQuery();
        /*
        static PolygonQuery::Ptr createQuery(entity::agro::AgroEntity::Ptr agro);

        static PolygonQuery::Ptr createQuery(entity::Polygon::Ptr agro);

        static PolygonQuery::Ptr createQuery(const geom::Polygon *polygon);
        */
        
        /**
         * @brief calculatePoints should this be done here?
         */        
        //int checkIntersection(entity::PointCloud::Point v11, entity::PointCloud::Point v12, entity::PointCloud::Point v21, entity::PointCloud::Point v22);
        int checkIntersection(geom::Coordinate v11, geom::Coordinate v12, geom::Coordinate v21, geom::Coordinate v22);

        const geom::Polygon *m_poly;

    };

//static const geom::GeometryFactory* m_factory = geom::GeometryFactory::getDefaultInstance();

}}

#endif /* end of include guard: SEMPR_QUERY_POLYGONQUERY_HPP_ */
