#ifndef SEMPR_ENTITY_ENTITY_H_
#define SEMPR_ENTITY_ENTITY_H_

#include <odb/core.hxx>

#include <string>
#include <vector>
#include <memory>

#include <boost/uuid/uuid_io.hpp>
#include <sempr/storage/DBObject.h>
#include <sempr/core/Event.h>
#include <sempr/core/EntityEvent.h>
// #include <sempr/core/Core.h>
// #include <sempr/core/EventBroker.h>


#define ENTITY_DEFAULT_CHANGED(CLASS) \
    virtual void changed() { \
        auto e = std::make_shared<sempr::core::EntityEvent<CLASS> >(  \
            shared_from_base<CLASS>(), sempr::core::EntityEventBase::CHANGED  \
        );  \
        fireEvent(e);  \
    }

#define ENTITY_DEFAULT_CREATED(CLASS) \
    virtual void created() { \
        auto e = std::make_shared<sempr::core::EntityEvent<CLASS> >(  \
            shared_from_base<CLASS>(), sempr::core::EntityEventBase::CREATED  \
        );  \
        fireEvent(e);  \
    }

#define ENTITY_DEFAULT_LOADED(CLASS) \
    virtual void loaded() { \
        auto e = std::make_shared<sempr::core::EntityEvent<CLASS> >(  \
            shared_from_base<CLASS>(), sempr::core::EntityEventBase::LOADED  \
        );  \
        fireEvent(e);  \
    }

#define ENTITY_DEFAULT_REMOVED(CLASS) \
    virtual void removed() { \
        auto e = std::make_shared<sempr::core::EntityEvent<CLASS> >(  \
            shared_from_base<CLASS>(), sempr::core::EntityEventBase::REMOVED  \
        );  \
        fireEvent(e);  \
    }

#define ENTITY_DEFAULT_EVENT_METHODS(class) \
    ENTITY_DEFAULT_CREATED(class) \
    ENTITY_DEFAULT_LOADED(class) \
    ENTITY_DEFAULT_CHANGED(class) \
    ENTITY_DEFAULT_REMOVED(class)


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
    using Event = core::EntityEvent<Entity>;
    virtual std::string id() const { return boost::uuids::to_string(uuid()); }
    virtual ~Entity(){}

    /** fires an event signalling that this entity changed.
        Needs to be overriden for sub-types to also fire special events, like
        RDFEvent, GeometryEvent, ... (TODO create those -- or instantiate?)
        (TODO: what to do on creation? --> created() virtual method!,
        same for removed(), loaded())
    */
    virtual void changed();
    virtual void created();
    virtual void loaded();
    virtual void removed();
    
protected:
    /** fires an event **/
    void fireEvent(core::Event::Ptr e);
    
    template<class Derived>
    std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }
    
private:
    friend class odb::access;
    friend class core::Core;
    
    /** used to fire events, set by the core **/
    #pragma db transient
    std::weak_ptr<core::EventBroker> broker_;
};

}}

#endif /* end of include guard: SEEPRO_ENTITY_ENTITY_H_ */
