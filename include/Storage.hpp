#ifndef SEMPR_STORAGE_STORAGE_H_
#define SEMPR_STORAGE_STORAGE_H_

#include "Entity.hpp"

#include <memory>
#include <vector>

namespace sempr {

/**
    Backend for storing entities and their data.
*/
class Storage {
public:
    using Ptr = std::shared_ptr<Storage>;
    using WPtr = std::weak_ptr<Storage>;
    virtual ~Storage(){};

    /** save data */
    virtual void save( Entity::Ptr data ) = 0;

    /** save bulk data */
    virtual void save( std::vector<Entity::Ptr>& data ) = 0;

    /** load a single object **/
    virtual Entity::Ptr load( const std::string& id ) = 0;

    /** load all objects **/
    virtual void loadAll( std::vector<Entity::Ptr>& data ) = 0;

    /** remove an object **/
    virtual void remove( Entity::Ptr data ) = 0;

    /** remove a bunch of objects */
    virtual void remove( std::vector<Entity::Ptr>& data ) = 0;
};

}

#endif /* end of include guard: SEMPR_STORAGE_STORAGE_H_ */
