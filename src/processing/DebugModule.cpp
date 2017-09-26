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
    std::cout << "EntityEvent::Ptr " << e->getEntity()->id() << '\n';
}

} /* processing */
    
} /* sempr */
