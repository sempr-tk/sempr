#include <sempr/processing/DebugModule.hpp>
#include <iostream>

namespace sempr { namespace processing {

DebugModule::DebugModule()
{
    addOverload<core::EntityEventBase >(
        [this](core::EntityEventBase::Ptr e) { process(e); }
    );
}

DebugModule::~DebugModule()
{
}

std::string DebugModule::type() const {
    return "DebugModule";
}

void DebugModule::process(core::Event::Ptr e) {
    std::cout << "DebugModule - Event::Ptr " << e->topic() << " " << e.get() << '\n';
}

void DebugModule::process(core::EntityEventBase::Ptr e) {
    switch(e->what()) {
        case core::EntityEventBase::CREATED:
            std::cout << "CREATED";
            break;
        case core::EntityEventBase::CHANGED:
            std::cout << "CHANGED";
            break;
        case core::EntityEventBase::REMOVED:
            std::cout << "REMOVED";
            break;
        case core::EntityEventBase::LOADED:
            std::cout << "LOADED";
            break;
        default:
            std::cout << "UNKNOWN EntityEvent";
    }
    std::cout << ": " << e->getEntity()->id() << '\n';
}

} /* processing */

} /* sempr */
