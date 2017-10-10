#include <sempr/core/Observer.h>
#include <iostream>

namespace sempr { namespace core {

Observer::Observer()
{
    addOverload<Event>(
        [this](Event::Ptr e) { process(e); }
    );
}

Observer::~Observer() 
{
}

void Observer::process(Event::Ptr e)
{
}

void Observer::notify(Event::Ptr event)
{
    if (types_.find(typeid(*event)) != types_.end()) {
        types_[typeid(*event)](event);
    } else {
        std::cout << typeid(*this).name() << " couldnt find " << typeid(*event).name() << '\n';
    }
}

} /* core */
    
} /* sempr */
