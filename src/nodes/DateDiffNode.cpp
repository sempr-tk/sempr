#include "nodes/DateDiffNode.hpp"
#include <rete-core/TupleWME.hpp>
#include <string>
#include <cmath>
#include <ctime>
#include <sstream>
#include <locale>
#include <iomanip>

namespace sempr {

DateDiffNode::DateDiffNode(
    rete::PersistentInterpretation<std::string> date1,
    rete::PersistentInterpretation<std::string> date2)
    :
        rete::Builtin("date:diff"),
        date1_(std::move(date1)),
        date2_(std::move(date2))
{
}

rete::WME::Ptr DateDiffNode::process(rete::Token::Ptr token)
{
    std::string date1, date2;
    std::string dateFormat = "%Y-%m-%d %H:%M:%S"; //TODO: make parameter

    date1_.interpretation->getValue(token, date1);
    date2_.interpretation->getValue(token, date2);

    std::tm a = {};
    std::tm b = {};

    std::istringstream ss1(date1);    
    ss1 >> std::get_time(&a, dateFormat.c_str());
    if (ss1.fail()){
        return nullptr;
    }

    std::istringstream ss2(date2);
    ss2 >> std::get_time(&b, dateFormat.c_str());
    if (ss2.fail()){
        return nullptr;
    }
    
    //Compute difference in days
    int dayDiff = 0;
    std::time_t t1 = std::mktime(&a);
    std::time_t t2 = std::mktime(&b);
    dayDiff = std::difftime(t2, t1) / (60 * 60 * 24); //Adjust to days    

    auto wme = std::make_shared<rete::TupleWME<int>>(dayDiff);

    return wme;
}

bool DateDiffNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const DateDiffNode*>(&other);
    if (!o) return false;

    return *(o->date1_.accessor) == *(this->date1_.accessor) && *(o->date2_.accessor) == *(this->date2_.accessor);
}

}
