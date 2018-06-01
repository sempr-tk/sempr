#ifndef SEMPR_QUERY_LOADINGQUERY_HPP_
#define SEMPR_QUERY_LOADINGQUERY_HPP_

#include <sempr/query/Query.hpp>
#include <sempr/storage/ODBStorage.hpp>
#include <odb/core.hxx>
#include <memory>

namespace sempr { namespace query {

    /**
        Loading-Queries are used to tell the DBUpdateModule to load certain entities from the
        database.

        NOTE: Sadly it only works with ODBStorage! There can be no virtual template methods...
    */
    class LoadingQueryBase : public Query, public core::OType<LoadingQueryBase> {
    public:
        using Ptr = std::shared_ptr<LoadingQueryBase>;
        virtual ~LoadingQueryBase(){}
        std::string type() const override { return "LoadingQuery"; }

        virtual void loadFrom(storage::ODBStorage::Ptr storage) = 0;
    };


    /**
        Query for all objects of the type specified as template parameter.

        TODO: Optional parameter for search-criteria? "Load only persons older than..." should be possible
    */
    template <class T>
    class LoadingQuery : public LoadingQueryBase, public core::OType<LoadingQuery<T>> {
    public:
        using Ptr = std::shared_ptr<LoadingQuery<T> >;
        std::vector<std::shared_ptr<T> > results;

        void loadFrom(storage::ODBStorage::Ptr storage) override
        {
            storage->loadAll(this->results);
        }

    };

}}

#endif /* end of include guard: SEMPR_QUERY_LOADINGQUERY_HPP_ */
