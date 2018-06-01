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
#include <vector>

namespace sempr { namespace core {

class Core {
public:
    Core();

    void addEntity(entity::Entity::Ptr entity);
    void removeEntity(entity::Entity::Ptr entity);


    /**
        Adds a module to the core, thus connecting it to events and queries.
    */
    void addModule(processing::ModuleBase::Ptr module);

    /**
        Returns the first added instance of the given module type.
        Does not remove the module from the core.
        \returns nullptr if no module of that type is present in the core
    */
    template <class ModuleType>
    std::shared_ptr<ModuleType> getModule()
    {
        for (auto m : modules_)
        {
            auto module = std::dynamic_pointer_cast<ModuleType>(m);
            if (module) return module;
        }
        return nullptr;
    }

    /**
        Returns a vector of all instances of the given module type.
        Does not remove the modules from the core.
    */
    template <class ModuleType>
    void listModules(std::vector<std::shared_ptr<ModuleType>>& list)
    {
        for (auto m : modules_)
        {
            auto module = std::dynamic_pointer_cast<ModuleType>(m);
            if (module) list.push_back(module);
        }
    }


    /**
        Let the core handle a query: Forwards the query to every module.
        TODO: Need a strategy to allow Modules asking queries, too, and to
        avoid endless loops when doing so.
    */
    void answerQuery(query::Query::Ptr query);

private:
    std::vector<processing::ModuleBase::Ptr> modules_;
    EventBroker::Ptr eventBroker_;
};

}}


#endif /* end of include guard: SEMPR_CORE_CORE_H_ */
