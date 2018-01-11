#ifndef SEMPR_STORAGE_ODBSTORAGE_H_
#define SEMPR_STORAGE_ODBSTORAGE_H_

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/schema-catalog.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/session.hxx>

#include <sempr/storage/Storage.hpp>

#include <memory>

namespace sempr { namespace storage {

/**
    Backend for storing entities and their data.

*/
class ODBStorage : public Storage {
public:
    using Ptr = std::shared_ptr<ODBStorage>;

    ODBStorage(const std::string& dbName = "sempr_sqlite.db", bool clearDatabase = false);
    ~ODBStorage();

    /** save data */
    void save( DBObject::Ptr data ) override;

    /** load a single object **/
    DBObject::Ptr load( const std::string& id ) override;

    template <typename T> std::shared_ptr<T> load( const std::string& id ) {

        static_assert(std::is_base_of<DBObject, T>::value,"Classes that are to be loaded from ODBStorage must inherit from DBObject");

        odb::transaction t( db_->begin() );
        std::shared_ptr<T> o( db_->load<T>(id) );
        t.commit();
        return o;
    }


    /**
        Load all instances of a given type.
    */
    template <typename T>
    void loadAll(std::vector<std::shared_ptr<T> >& list) {
        odb::transaction t(db_->begin());
        odb::result<T> r = db_->query<T>();
        for (auto it = r.begin(); it != r.end(); ++it)
        {
            list.push_back(it.load());
        }
        t.commit();
    }

    /** load all objects **/
    void loadAll( std::vector<DBObject::Ptr>& data ) override;

    /** remove an object **/
    void remove( DBObject::Ptr data ) override;

private:
    std::unique_ptr<odb::database> db_;
    std::unique_ptr<odb::session> session_;


};

}}

#endif /* end of include guard: SEMPR_STORAGE_ODBSTORAGE_H_ */
