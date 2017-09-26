#include <sempr/core/EntityEvent.h>

namespace sempr { namespace core {

EntityEvent::EntityEvent(entity::Entity::Ptr entity, EventType type)
    : entity_(entity), type_(type)
{
}

EntityEvent::~EntityEvent()
{
}


entity::Entity::Ptr EntityEvent::getEntity() {
    return entity_;
}

EntityEvent::EventType EntityEvent::what() {
    return type_;
}


}}
