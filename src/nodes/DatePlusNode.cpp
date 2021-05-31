#include "nodes/DatePlusNode.hpp"
#include <rete-core/TupleWME.hpp>
#include <string>
#include <cmath>
#include <ctime>
#include <sstream>
#include <locale>
#include <iomanip>
#include <iostream>

namespace sempr {

DatePlusNode::DatePlusNode(
    rete::PersistentInterpretation<std::string> date,
    rete::PersistentInterpretation<int> days)
    :
        rete::Builtin("date:plus"),
        date_(std::move(date)),
        days_(std::move(days))
{
}

//Compute the sum date + days
rete::WME::Ptr DatePlusNode::process(rete::Token::Ptr token)
{
    std::string date;
    int days;
    std::string dateFormat = "%Y-%m-%d %H:%M:%S"; //TODO: make parameter

    date_.interpretation->getValue(token, date);
    days_.interpretation->getValue(token, days);

    std::tm tm_ = {};

    std::istringstream ss1(date);    
    ss1 >> std::get_time(&tm_, dateFormat.c_str());
    if (ss1.fail()){
        return nullptr;
    }
            
    //Compute sum
    std::time_t t1 = std::mktime(&tm_);    
    auto sum = t1 + days*(24 * 60 * 60);
    
    struct tm * timeinfo;
    char buffer [80];    
    timeinfo = localtime (&sum); //TODO: is localtime OK? It considers daylight savings, so...
    strftime (buffer, 80, dateFormat.c_str(), timeinfo);
    
    std::string newDate = buffer;
    
    //std::cout << date << " + " << days << " days = " << newDate << std::endl;

    auto wme = std::make_shared<rete::TupleWME<std::string>>(newDate);

    return wme;
}

bool DatePlusNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const DatePlusNode*>(&other);
    if (!o) return false;

    return *(o->date_.accessor) == *(this->date_.accessor) && *(o->days_.accessor) == *(this->days_.accessor);
}

}
