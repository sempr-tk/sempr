#ifndef SEMPR_PROCESSING_DBUPDATEMODULE_H_
#define SEMPR_PROCESSING_DBUPDATEMODULE_H_

#include <Entity_odb.h>

#include <sempr/core/EntityEvent.hpp>
#include <sempr/processing/Module.hpp>
#include <sempr/storage/Storage.hpp>
#include <sempr/entity/Entity.hpp>
#include <vector>

namespace sempr { namespace processing {

/**
    This module uses a database storage module to persist entities that are used within the system.
    It follows a single-persist-bulk-update-strategy:
    Whenever a new entity is created (i.e., entity->created() is called), the DBUpdateModule will
    immediately call storage_->save(entity) to persist it. But, whenever an entity is changed or
    removed (i.e., entity->changed()/removed() is called), the change will not be transmitted to the
    database until the user calls DBUpdateModule::updateDabase(). The updateDatabase-method will
    process all changes and removals in one transaction, thus increasing performance.

    The different handling of created-events is done to always keep the database in a valid state:
    Think of a small example with two entities in which one points to the other:
        a->created();
        b->created();
        a->pointTo(b); a->changed();
    If we start persisting the entities only after a is set to point to b, we will get a foreign key
    constraint violation upon saving a (and its reference to b), as b is not stored in the db yet.

    One can argue that this is an abstract, theoretical problem, because due to the automation of
    persistence we are (quite) sure that "b" will be created right after "a" -- unless the user missed
    adding b to the system, in which case an exception describing the constraint violation is the
    right response.

    TODO: Add functionality to allow the user bulk-creation of entities, on his own risk. Something like "DBUpdateModule::enableBulkCreation(bool)".
*/
class DBUpdateModule : public Module<core::EntityEvent<entity::Entity>> {
public:
    using Ptr = std::shared_ptr<DBUpdateModule>;
    DBUpdateModule(storage::Storage::Ptr storage);

    /** upon receiving a changed-event, update the database */
    void process(core::EntityEvent<entity::Entity>::Ptr e) override;


    /** save the changed and erase the entities flagged for removal */
    void updateDatabase();

private:
    storage::Storage::Ptr storage_;

    /// list of changed entities.
    std::vector<storage::DBObject::Ptr> toUpdate_;
    /// list of entities that need to be removed.
    std::vector<storage::DBObject::Ptr> toRemove_;

    /**
        Note: registerAdded is a bit misleading. In the current implementation, this directly
        persists the object in the database.
    */
    void registerAdded(storage::DBObject::Ptr);

    /**
        Registers an object to be saved on the next call of updateDatabase().
        Throws a sempr::Exception if the object has been flagged for removal before.
    */
    void registerChanged(storage::DBObject::Ptr);

    /**
        Registers an object to be removed on the next call of updateDatabase().
    */
    void registerRemoved(storage::DBObject::Ptr);
};

}}

#endif /* end of include guard: SEMPR_PROCESSING_DBUPDATEMODULE_H_ */
