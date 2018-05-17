#ifndef SEMPR_QUERY_OBJECTQUERY_HPP_
#define SEMPR_QUERY_OBJECTQUERY_HPP_

#include <sempr/query/Query.hpp>
#include <sempr/storage/DBObject.hpp>
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
    class ObjectQueryBase : public Query, public core::OType<ObjectQueryBase> {
    public:
        using Ptr = std::shared_ptr<ObjectQueryBase>;
        virtual ~ObjectQueryBase(){}
        std::string type() const override { return "ObjectQuery"; }

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
        Query for all objects of the type specified as template parameter. As an optional parameter you may pass a function
        with the signature
            bool(std::shared_ptr<T>)
        that will be used to check if an entity of type T should
        be returned in the result list.

        Example:
        // to get all persons:
        auto query = std::make_shared<ObjectQuery<Person> >();

        // to get all persons of age 25 and above
        auto query = std::make_shared<ObjectQuery<Person> >(
            [](Person::Ptr p) { return p->age() >= 25; }
        );
    */
    template <class T>
    class ObjectQuery : public ObjectQueryBase, public core::OType<ObjectQuery<T>> {
    private:
        typedef std::function<bool(std::shared_ptr<T>)> DecisionFunction_t;
        DecisionFunction_t decision_;
    public:
        using Ptr = std::shared_ptr<ObjectQuery<T> >;
        std::vector<std::shared_ptr<T> > results;

        ObjectQuery(DecisionFunction_t decisionFunction = nullptr)
            : decision_(decisionFunction)
        {
        }

        /// helper method to create queries with a decision-function more easily.
        // template <typename X>
        // static typename ObjectQuery<T, X>::Ptr create(X decisionFunction)
        // {
        //     typename ObjectQuery<T, X>::Ptr query(new ObjectQuery<T, X>(decisionFunction));
        //     return query;
        // }

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
                std::shared_ptr<T> o = std::static_pointer_cast<T>(object);
                // add it to the result if there is no further criterion or the given decision
                // function returns true
                if (!decision_ || decision_(o))
                {
                    results.push_back(o);
                }
            }
        }

    };

}}

#endif /* end of include guard: SEMPR_QUERY_OBJECTQUERY_HPP_ */
