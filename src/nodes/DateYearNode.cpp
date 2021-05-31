#include "nodes/DateYearNode.hpp"
#include <rete-core/TupleWME.hpp>

#include <string>
#include <iostream>
#include <ctime>
#include <sstream>
#include <locale>
#include <iomanip>

namespace sempr {

DateYearNode::DateYearNode(
    rete::PersistentInterpretation<std::string> date)
    :
        rete::Builtin("date:year"),
        date_(std::move(date))
{
}

rete::WME::Ptr DateYearNode::process(rete::Token::Ptr token)
{
    std::string date;
    date_.interpretation->getValue(token, date);
    std::string dateFormat = "%Y-%m-%d %H:%M:%S"; //TODO: make parameter
    
    //std::cout << "date:Year | token = " << date << std::endl;
    std::tm tm_ = {};
    std::istringstream ss1(date);
    ss1 >> std::get_time(&tm_, dateFormat.c_str());
    if (ss1.fail()){
        return nullptr;
    }

    int year = tm_.tm_year + 1900;
    //std::cout << "date:Year | Year(int) is " << Year << std::endl;

    auto wme = std::make_shared<rete::TupleWME<int>>(year);

    return wme;
}

bool DateYearNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const DateYearNode*>(&other);
    if (!o) return false;

    return *(o->date_.accessor) == *(this->date_.accessor);
}

}
