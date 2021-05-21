#include "nodes/DateMonthNode.hpp"
#include <rete-core/TupleWME.hpp>

#include <string>
#include <iostream>
#include <ctime>
#include <sstream>
#include <locale>
#include <iomanip>

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
    std::string dateFormat = "%Y-%m-%d %H:%M:%S"; //TODO: make parameter

    //std::cout << "date:month | token = " << date << std::endl;

    std::tm tm_ = {};
    std::istringstream ss1(date);
    ss1 >> std::get_time(&tm_, dateFormat.c_str());
    if (ss1.fail()){
        return nullptr;
    }

    int month = tm_.tm_mon + 1;

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
