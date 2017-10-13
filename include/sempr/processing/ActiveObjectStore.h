#ifndef SEMPR_PROCESSING_ACTIVEOBJECTSTORE_H_
#define SEMPR_PROCESSING_ACTIVEOBJECTSTORE_H_

#include <sempr/processing/Module.h>
#include <sempr/entity/Entity.h>


namespace sempr { namespace processing {

/**
    The ActiveObjectStore is a module that listens for created/loaded or removed
    entities and updates an internal map.
    TODO:
        - decide how to use this module
            - just put it into the set of all modules?
            - give it a special place, to use it directly in the core?
            - "dual-interface": query and methods?
        - design & implement query
            - get by id / uuid
            - list by class (how? register before using?)
            - in general: how to implement queries?
                - again, map type --> method?
                - how to server templated queries (--> contain class type!)?
*/
class ActiveObjectStore : public Module {
public:
    using Ptr = std::shared_ptr<ActiveObjectStore>;
    ActiveObjectStore();
    virtual ~ActiveObjectStore();
    virtual std::string type() const override;
    
    void process(entity::Entity::Event::Ptr event);
    
    void printStats() const {
        // gather #objects per class
        std::map<std::type_index, size_t> perClass;
        for (auto p : entities_) {
            perClass[typeid(*p.second)]++;
        }
        
        std::cout << "--- ActiveObjectStore ---" << '\n';
        std::cout << "#total entities: " << entities_.size() << '\n';
        std::cout << "#classes:        " << perClass.size() << '\n';
        std::cout << "classes:" << '\n';
        for (auto p : perClass) {
            std::cout << "  #" << p.first.name() << ":    " << p.second << '\n';
        }
        std::cout << "entities:" << '\n';
        for(auto p : entities_) {
            std::cout << "  " << p.first << '\n';
        }
    }
    
private:
    // map entity-id to entity
    std::map<std::string, entity::Entity::Ptr> entities_;
    
    void addEntity(entity::Entity::Ptr e);
    void removeEntity(entity::Entity::Ptr e);
};

} /* processing */
    
} /* sempr */


#endif /* end of include guard: SEMPR_PROCESSING_ACTIVEOBJECTSTORE_H_ */
