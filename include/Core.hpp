#ifndef SEMPR_CORE_CORE_H_
#define SEMPR_CORE_CORE_H_

//#include <sempr/query/Query.hpp>
#include "Entity.hpp"
#include "IDGenerator.hpp"
#include "Storage.hpp"

#include <rete-reasoner/Reasoner.hpp>
//#include <sempr/storage/Storage.hpp>
//#include <sempr/processing/ModuleBase.hpp>
#include <string>
#include <map>
#include <set>
#include <memory>
#include <vector>

namespace sempr {

class Core {
    /// The reasoner is the backbone of sempr, used for all kinds of event handling
    rete::Reasoner reasoner_;

    /// a set of all added entities, in order to unset their Core* when the core
    // is deconstructed.
    std::set<Entity::Ptr> entities_;

    /// An id generator to assign ids to entities that are added to the core
    IDGenerator::Ptr idgen_;

    /// A storage module to persist entities
    Storage::Ptr storage_;
public:
    Core();
    Core(IDGenerator::Ptr idgen);
    Core(IDGenerator::Ptr idgen, Storage::Ptr storage);

    ~Core();

    /**
        Grant access to the internal reasoner, in order to register NodeBuilders
        and load/unload rules.
    */
    rete::Reasoner& reasoner();

    /**
        Access to the storage module
    */
    Storage::Ptr storage();

    /**
        Adds an entity by basically adding <Entity, Component>
        WMEs to the reasoner.
    */
    void addEntity(Entity::Ptr entity);

    /**
        Removes the entity from the reasoner, by removing the evidence for the
        <Entity, Component> WMEs.
    */
    void removeEntity(Entity::Ptr entity);


    /**
        Notifies the reasoner that the entity has a new component.
        This is automatically called by the entity.
    */
    void addedComponent(Entity::Ptr, Component::Ptr);

    /**
        Notifies the reasoner that the entity lost a component.
        This is automatically called by the entity.
    */
    void removedComponent(Entity::Ptr, Component::Ptr);

    /**
        Notifies the reasoner that a component of an entity has changed.
        This is automatically called by the component.
    */
    void changedComponent(Entity::Ptr, Component::Ptr);


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

}


#endif /* end of include guard: SEMPR_CORE_CORE_H_ */

