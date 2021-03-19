#include "nodes/DateMonthNode.hpp"
#include <rete-core/TupleWME.hpp>

#include <string>
#include <iostream>

namespace sempr {

DateMonthNode::DateMonthNode(
    rete::PersistentInterpretation<std::string> date)
    :
        rete::Builtin("date:month"),
        date_(std::move(date))
{
}

rete::WME::Ptr DateMonthNode::process(rete::Token::Ptr token)
{
    std::string date;
    date_.interpretation->getValue(token, date);

    //std::cout << "date:month | token = " << date << std::endl;

    int month = stoi(date.substr(5,2)); //month is always in field 5 and is length 2

    //std::cout << "date:month | month(int) is " << month << std::endl;

    auto wme = std::make_shared<rete::TupleWME<int>>(month);

    return wme;
}

bool DateMonthNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const DateMonthNode*>(&other);
    if (!o) return false;

    return *(o->date_.accessor) == *(this->date_.accessor);
}

}
