#include <sempr/core/EventBroker.hpp>
#include <algorithm>
#include <iostream>

namespace sempr { namespace core {

void EventBroker::enqueueEvent(Event::Ptr event) {
    // if (event->topic().empty()) {
        // event->topic("t_all");
    // } else {
        event->topic("t_" + event->topic());
    // }

    eventQueue.push(event);

    processEvent(); // process events as soon as they are fired
}

void EventBroker::addObserver(Observer::Ptr observer, const std::string& topic) {
    // don't add if the observer is already in "all" list
    // auto it = std::find(observers["all"].begin(), observers["all"].end(), observer);
    auto it = std::find_if(observers["all"].begin(), observers["all"].end(),
        [observer](std::weak_ptr<Observer> other) {
            Observer::Ptr o = other.lock();
            return (o.get() == observer.get());
        }
    );

    if (it == observers["all"].end()) {
        observers["t_" + topic].push_back(observer);
    }
}

void EventBroker::addObserver(Observer::Ptr observer) {
    observers["all"].push_back(observer);

    // remove from any other topic to prevent multiple notifications for the
    // same event
    for (auto kv : observers) {
        if (kv.first != "all") {
            auto it = std::find_if(kv.second.begin(), kv.second.end(),
                [observer](std::weak_ptr<Observer> other) {
                    Observer::Ptr o = other.lock();
                    return (o.get() == observer.get());
                }
            );
            if (it != kv.second.end()) kv.second.erase(it);
        }
    }
}

void EventBroker::processEvent() {
    if (!eventQueue.empty()) {
        Event::Ptr event = eventQueue.front();

        // std::cout << "EventBroker::processEvent " << typeid(*event).name() << '\n';

        // notify all observers registered to the topic
        for (auto o : observers[event->topic()]) {
            Observer::Ptr observer = o.lock();
            if (observer.get()) {
                observer->notify(event);
            } // TODO: else remove invalid weak-ptr from list.
        }

        // notify all observers registered to "all" topics
        // for (Observer::Ptr o : observers["all"]) {
        for (auto o : observers["all"]) {
            Observer::Ptr observer = o.lock();
            if (observer.get()) {
                observer->notify(event);
            }
        }

        eventQueue.pop();
    }
}

} /* core */
} /* sempr */
