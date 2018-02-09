#ifndef SEMPR_PROCESSING_DBUPDATEMODULE_H_
#define SEMPR_PROCESSING_DBUPDATEMODULE_H_

#include <sempr/core/EntityEvent.hpp>
#include <sempr/processing/Module.hpp>
#include <sempr/storage/Storage.hpp>
#include <sempr/entity/Entity.hpp>
#include <vector>

namespace sempr { namespace processing {

class DBUpdateModule : public Module {
public:
    using Ptr = std::shared_ptr<DBUpdateModule>;
    DBUpdateModule(storage::Storage::Ptr storage);

    /** upon receiving a changed-event, update the database */
    void process(entity::Entity::Event::Ptr e);

    void persistEntities();

    void persistEntitiesInBulk();

private:
    storage::Storage::Ptr storage_;

    // list of newly added entities, not yet persistet
    std::vector<storage::DBObject::Ptr> newEntities_;

    void addNewEntity(entity::Entity::Ptr e);
    void addEntity(entity::Entity::Ptr e);
    void removeEntity(entity::Entity::Ptr e);
};

}}

#endif /* end of include guard: SEMPR_PROCESSING_DBUPDATEMODULE_H_ */
