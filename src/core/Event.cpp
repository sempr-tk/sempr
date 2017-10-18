#include <sempr/core/Event.hpp>

namespace sempr { namespace core {

Event::Event() : topic_(""), sender_("")
{
}

Event::~Event()
{
}


std::string Event::topic() const { return topic_; }
void Event::topic(const std::string& topic) { topic_ = topic; }

std::string Event::sender() const { return sender_; }
void Event::sender(const std::string& sender) { sender_ = sender; }



} /* core */
    
} /* sempr */
