#ifndef SEMPR_CORE_CORE_H_
#define SEMPR_CORE_CORE_H_

#include <sempr/core/Event.h>
#include <sempr/core/EventBroker.h>
#include <sempr/core/Observer.h>
#include <sempr/query/Query.h>
#include <sempr/entity/Entity.h>

#include <sempr/storage/Storage.h>
#include <sempr/processing/Module.h>
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
    
    
    void addModule(processing::Module::Ptr module);
    EventBroker::Ptr getEventBroker() { return eventBroker_; }
    
    // TODO queries stellen
    // wie unterscheidet sich eine SPARQL-Query (Query-Modul)
    // von einer RDF-Query (Reasoning/Processing-Module)?
    // Sind die Query-Module im Endeffekt in der gleichen Struktur angesiedelt,
    // wie Processing-Module? (Mein Eindruck: Ja.)
    // void answerQuery(Query::Ptr query);
    
private:
    /// map entity-id to entity
    std::map<std::string, entity::Entity::Ptr> entities_;
    
    storage::Storage::Ptr storage_;
    std::vector<processing::Module::Ptr> modules_;
    EventBroker::Ptr eventBroker_;
};
    
}}


#endif /* end of include guard: SEMPR_CORE_CORE_H_ */
