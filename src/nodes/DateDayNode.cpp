#include "nodes/DateDayNode.hpp"
#include <rete-core/TupleWME.hpp>

#include <string>
#include <cmath>
#include <ctime>
#include <sstream>
#include <locale>
#include <iomanip>

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
    std::string dateFormat = "%Y-%m-%d %H:%M:%S"; //TODO: make parameter

    //std::cout << "date:day | token = " << date << std::endl;

    std::tm tm_ = {};
    std::istringstream ss1(date);
    ss1 >> std::get_time(&tm_, dateFormat.c_str());
    if (ss1.fail()){
        return nullptr;
    }

    int day = tm_.tm_mday;

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
