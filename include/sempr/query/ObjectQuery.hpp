#ifndef SEMPR_QUERY_OBJECTQUERY_HPP_
#define SEMPR_QUERY_OBJECTQUERY_HPP_

#include <sempr/query/Query.hpp>
#include <sempr/core/DBObject.hpp>
#include <odb/core.hxx>
#include <memory>

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
    class ObjectQueryBase : public Query {
    public:
        using Ptr = std::shared_ptr<ObjectQueryBase>;
        virtual ~ObjectQueryBase(){}

        /**
            Every derived class of ObjectQueryBase may implement its own
            criteria if an object does or does not match, and how to present
            the results. The ActiveObjectStore only needs to call "consider" for
            every active object.
        */
        virtual void consider(storage::DBObject::Ptr) = 0;
    private:

    };



    /**
        Query for all objects of the type specified as template parameter.
    */
    template <class T>
    class ObjectQuery : public ObjectQueryBase {
    public:
        std::vector<std::shared_ptr<T> > results;

        void consider(storage::DBObject::Ptr object) override
        {
            /** v1: dynamic cast */
            // std::shared_ptr<T> derived = std::dynamic_pointer_cast<T>(object);
            // if (derived) {
            //     results.push_back(derived);
            // }

            /** v2: static cast + simplified check on discriminator */
            if (odb::object_traits_impl<T, odb::id_common>::info.discriminator
                == object->discriminator())
            {
                results.push_back(std::static_pointer_cast<T>(object);
            }
        }

    };

}}

#endif /* end of include guard: SEMPR_QUERY_OBJECTQUERY_HPP_ */
