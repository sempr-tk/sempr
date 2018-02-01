#ifndef SEMPR_PROCESSING_ACTIVEOBJECTSTORE_H_
#define SEMPR_PROCESSING_ACTIVEOBJECTSTORE_H_

#include <sempr/processing/Module.hpp>
#include <sempr/entity/Entity.hpp>
#include <sempr/query/ObjectQuery.hpp>
#include <sempr/storage/Storage.hpp>

#include <iostream>


namespace sempr { namespace processing {

/**
    The ActiveObjectStore is a module that listens for created/loaded or removed
    entities and updates an internal map.
    TODO: We might want to be able to remove some objects just from the active
    list (something like an "unload"). When implementing this: Remember to also
    remove the object from the current odb::session!
*/
class ActiveObjectStore : public Module {
public:
    using Ptr = std::shared_ptr<ActiveObjectStore>;

    ActiveObjectStore(storage::Storage::Ptr storage);
    virtual ~ActiveObjectStore();

    virtual std::string type() const override;

    /**
        Keep a list of currently used objects -- add on "created" or "loaded",
        remove on "removed".
    */
    void process(entity::Entity::Event::Ptr event);

    /**
        Override the default answer-method to allow handling of queries that
        are derived from ObjectQueryBase.
    */
    void answer(query::Query::Ptr query) override;

    /**
        Answer an ObjectQuery.
    */
    void answer(query::ObjectQueryBase::Ptr query);

    void printStats(bool showEntities = false) const {
        // gather #objects per class
        std::map<std::string, size_t> perClass;
        for (auto p : entities_) {
            perClass[p.second->discriminator()]++;
        }

        std::cout << "--- ActiveObjectStore ---" << '\n';
        std::cout << "# total entities:\t" << entities_.size() << '\n';
        std::cout << "# not persited:  \t" << newEntities_.size() << '\n';
        std::cout << "# classes:       \t" << perClass.size() << '\n';
        std::cout << "classes:" << '\n';
        for (auto p : perClass) {
            std::cout << "  # " << p.first << ":    " << p.second << '\n';
        }
        if (showEntities) {
            std::cout << "entities:" << '\n';
            for(auto p : entities_) {
                std::cout << "  " << p.first << '\n';
            }
        }
        std::cout << "-------------------------" << std::endl;
    }

    void persistEntities();

    void persistEntitiesInBulk();

private:
    // list of newly added entities, not yet persistet
    std::vector<storage::DBObject::Ptr> newEntities_;
    // map entity-id to entity
    std::map<std::string, entity::Entity::Ptr> entities_;
    // pointer to the used storage
    storage::Storage::Ptr storage_;

    void addNewEntity(entity::Entity::Ptr e);
    void addEntity(entity::Entity::Ptr e);
    void removeEntity(entity::Entity::Ptr e);
};

} /* processing */

} /* sempr */


#endif /* end of include guard: SEMPR_PROCESSING_ACTIVEOBJECTSTORE_H_ */
