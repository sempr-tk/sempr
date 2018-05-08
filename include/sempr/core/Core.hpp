#ifndef SEMPR_CORE_CORE_H_
#define SEMPR_CORE_CORE_H_

#include <sempr/core/Event.hpp>
#include <sempr/core/EventBroker.hpp>
#include <sempr/core/Observer.hpp>
#include <sempr/query/Query.hpp>
#include <sempr/entity/Entity.hpp>

#include <sempr/storage/Storage.hpp>
#include <sempr/processing/ModuleBase.hpp>
#include <string>
#include <map>
#include <memory>

namespace sempr { namespace core {

class Core {
public:
    Core(storage::Storage::Ptr backend);

    void addEntity(entity::Entity::Ptr entity);
    void removeEntity(entity::Entity::Ptr entity);
    // void removeEntity(const std::string& entity_id);


    void addModule(processing::ModuleBase::Ptr module);

    /**
        Let the core handle a query: Forwards the query to every module.
        TODO: Need a strategy to allow Modules asking queries, too, and to
        avoid endless loops when doing so.
    */
    void answerQuery(query::Query::Ptr query);

private:
    /// map entity-id to entity
    std::map<std::string, entity::Entity::Ptr> entities_;

    storage::Storage::Ptr storage_;
    std::vector<processing::ModuleBase::Ptr> modules_;
    EventBroker::Ptr eventBroker_;
};

}}


#endif /* end of include guard: SEMPR_CORE_CORE_H_ */
