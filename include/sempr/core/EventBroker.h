#ifndef SEMPR_CORE_EVENTBROKER_H_
#define SEMPR_CORE_EVENTBROKER_H_

#include <sempr/core/Event.h>
#include <sempr/core/Observer.h>
#include <memory>
#include <string>
#include <queue>
#include <vector>
#include <map>

namespace sempr { namespace core {
    
class EventBroker {
public:
    using Ptr = std::shared_ptr<EventBroker>;
    
    void enqueueEvent(Event::Ptr event);
    void addObserver(Observer::Ptr observer);
    void addObserver(Observer::Ptr observer, const std::string& topic);
    
    void processEvent();
    void processAllEvents();
    
private:
    std::queue<Event::Ptr> eventQueue;
    std::map<std::string, std::vector<Observer::Ptr> > observers;
    
};


} /* core */
} /* sempr */



#endif /* end of include guard: SEMPR_CORE_EVENTBROKER_H_ */
