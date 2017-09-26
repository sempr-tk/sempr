#ifndef SEMPR_STORAGE_ODBSTORAGE_H_
#define SEMPR_STORAGE_ODBSTORAGE_H_

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/schema-catalog.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/session.hxx>

#include <sempr/storage/Storage.h>

#include <memory>

namespace sempr { namespace storage {

/**
    Backend for storing entities and their data.
    
*/
class ODBStorage : public Storage {
public:
    using Ptr = std::shared_ptr<ODBStorage>;
    
    ODBStorage();
    ~ODBStorage();
    
    /** save data */
    void save( DBObject::Ptr data ) override;
    
    /** load a single object **/
    DBObject::Ptr load( const boost::uuids::uuid& id ) override;
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
