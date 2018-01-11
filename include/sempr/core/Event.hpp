#ifndef SEMPR_CORE_EVENT_H_
#define SEMPR_CORE_EVENT_H_

#include <string>
#include <memory>

#include <sempr/core/Observable.hpp>

namespace sempr { namespace core {

class Event : public Observable {
public:
    using Ptr = std::shared_ptr<Event>;

    Event();
    virtual ~Event();

    std::string topic() const;
    void topic(const std::string& topic);

    /** if an observer decides to throw an event itself, it may want to
        recognize it later on.
    */
    std::string sender() const;
    void sender(const std::string& sender);


protected:
    std::string topic_;
    std::string sender_;
};

}}

#endif /* end of include guard: SEMPR_CORE_EVENT_H_ */
