#ifndef SEMPR_CORE_CORE_H_
#define SEMPR_CORE_CORE_H_

//#include <sempr/query/Query.hpp>
#include "sempr/entity/Entity.hpp"
#include "sempr/core/IDGenerator.hpp"

#include <rete-reasoner/Reasoner.hpp>
//#include <sempr/storage/Storage.hpp>
//#include <sempr/processing/ModuleBase.hpp>
#include <string>
#include <map>
#include <set>
#include <memory>
#include <vector>

namespace sempr { namespace core {

class Core {
    /// The reasoner is the backbone of sempr, used for all kinds of event handling
    rete::Reasoner reasoner_;

    /// a set of all added entities, in order to unset their Core* when the core
    // is deconstructed.
    std::set<entity::Entity::Ptr> entities_;

    /// An id generator to assign ids to entities that are added to the core
    IDGenerator::Ptr idgen_;
public:
    Core();
    Core(IDGenerator::Ptr idgen);
    ~Core();

    /**
        Adds an entity by basically adding <Entity, Component, String-Tag>
        WMEs to the reasoner.
    */
    void addEntity(entity::Entity::Ptr entity);

    /**
        Removes the entity from the reasoner, by removing the evidence for the
        <Entity, Component, String-Tag> WMEs.
    */
    void removeEntity(entity::Entity::Ptr entity);

    // TODO add/remove single components to/from an entity?
   

    // TODO: Add "modules"? Query-/Interface-Modules/-Nodes?

//    /**
//        Returns the first added instance of the given module type.
//        Does not remove the module from the core.
//        \returns nullptr if no module of that type is present in the core
//    */
//    template <class ModuleType>
//    std::shared_ptr<ModuleType> getModule()
//    {
//        for (auto m : modules_)
//        {
//            auto module = std::dynamic_pointer_cast<ModuleType>(m);
//            if (module) return module;
//        }
//        return nullptr;
//    }
//
//    /**
//        Returns a vector of all instances of the given module type.
//        Does not remove the modules from the core.
//    */
//    template <class ModuleType>
//    void listModules(std::vector<std::shared_ptr<ModuleType>>& list)
//    {
//        for (auto m : modules_)
//        {
//            auto module = std::dynamic_pointer_cast<ModuleType>(m);
//            if (module) list.push_back(module);
//        }
//    }
//
//
//    /**
//        Let the core handle a query: Forwards the query to every module.
//        TODO: Need a strategy to allow Modules asking queries, too, and to
//        avoid endless loops when doing so.
//    */
//    void answerQuery(query::Query::Ptr query);

private:
//    std::vector<processing::ModuleBase::Ptr> modules_;
};

}}


#endif /* end of include guard: SEMPR_CORE_CORE_H_ */
