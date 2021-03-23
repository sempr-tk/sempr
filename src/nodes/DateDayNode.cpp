#include "nodes/DateDayNode.hpp"
#include <rete-core/TupleWME.hpp>
#include <string>

namespace sempr {

DateDayNode::DateDayNode(
    rete::PersistentInterpretation<std::string> date)
    :
        rete::Builtin("date:day"),
        date_(std::move(date))
{
}

rete::WME::Ptr DateDayNode::process(rete::Token::Ptr token)
{
    std::string date;
    date_.interpretation->getValue(token, date);

    //std::cout << "date:day | token = " << date << std::endl;

    int day = stoi(date.substr(8,2));

    //std::cout << "date:day | Day(int) is " << Day << std::endl;

    auto wme = std::make_shared<rete::TupleWME<int>>(day);

    return wme;
}

bool DateDayNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const DateDayNode*>(&other);
    if (!o) return false;

    return *(o->date_.accessor) == *(this->date_.accessor);
}

}
