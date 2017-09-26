#ifndef SEMPR_ENTITY_ENTITY_H_
#define SEMPR_ENTITY_ENTITY_H_

#include <odb/core.hxx>

#include <string>
#include <vector>
#include <memory>

#include <boost/uuid/uuid_io.hpp>
#include <sempr/storage/DBObject.h>
#include <sempr/core/Event.h>
// #include <sempr/core/EntityEvent.h>
// #include <sempr/core/Core.h>
// #include <sempr/core/EventBroker.h>



namespace sempr { 
    namespace core {
        class Core;
        class EventBroker;
        // class Event;
    }
     
namespace entity {

/**
    Collection of data with application-specific semantics.
*/
#pragma db object
class Entity : public storage::DBObject, public std::enable_shared_from_this<Entity>  {
public:
    Entity(){}
    using Ptr = std::shared_ptr<Entity>;
    virtual std::string id() const { return boost::uuids::to_string(uuid()); }
    virtual ~Entity(){}

    /** fires an event signalling that this entity changed **/
    void changed();
    
protected:
    /** fires an event **/
    void fireEvent(core::Event::Ptr e);
    
private:
    friend class odb::access;
    friend class core::Core;
    
    /** used to fire events, set by the core **/
    #pragma db transient
    std::weak_ptr<core::EventBroker> broker_;
    
    template<class Derived>
    std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }
};

}}

#endif /* end of include guard: SEEPRO_ENTITY_ENTITY_H_ */
