#ifndef SEMPR_STORAGE_STORAGE_H_
#define SEMPR_STORAGE_STORAGE_H_

#include <sempr/storage/DBObject.hpp>
#include <DBObject_odb.h>
// #include <sempr/storage/DBObject_odb.h>

#include <memory>
#include <vector>

namespace sempr { 
    namespace storage {

/**
    Backend for storing entities and their data.
*/
class Storage {
public:
    using Ptr = std::shared_ptr<Storage>;
    virtual ~Storage(){};
    
    /** save data */
    virtual void save( DBObject::Ptr data ) = 0;
    
    /** load a single object **/
    virtual DBObject::Ptr load( const boost::uuids::uuid& id ) = 0;
    /** load all objects **/
    virtual void loadAll( std::vector<DBObject::Ptr>& data ) = 0;
    
    /** remove an object **/
    virtual void remove( DBObject::Ptr data ) = 0;
    
protected:
    void setID( DBObject::Ptr object, const boost::uuids::uuid& id ) {
        object->id_ = id;
    }

};
    
}}

#endif /* end of include guard: SEMPR_STORAGE_STORAGE_H_ */
