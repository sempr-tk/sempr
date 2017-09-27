#include <sempr/processing/DebugModule.h>

#include <iostream>

namespace sempr { namespace processing {

DebugModule::DebugModule()
{
    addOverload<core::EntityEvent>(
        [this](core::EntityEvent::Ptr e) { process(e); }
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

void DebugModule::process(core::EntityEvent::Ptr e) {
    switch(e->what()) {
        case core::EntityEvent::CREATED:
            std::cout << "CREATED";
            break;
        case core::EntityEvent::CHANGED:
            std::cout << "CHANGED";
            break;
        case core::EntityEvent::REMOVED:
            std::cout << "REMOVED";
            break;
        case core::EntityEvent::LOADED:
            std::cout << "LOADED";
            break;
        default:
            std::cout << "UNKNOWN EntityEvent";
    }
    std::cout << ": " << e->getEntity()->id() << '\n';
}

} /* processing */
    
} /* sempr */
