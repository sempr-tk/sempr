#include "nodes/DateDiffNode.hpp"
#include <rete-core/TupleWME.hpp>
#include <string>
#include <cmath>
#include <ctime>
#include <regex>

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

/*
    Check whether a Date string has the format "YYYY(-|/|.)MM(-|/|.)DD HH:MM:SS"
    Implicitly verifies the length as well
*/
bool DateDiffNode::validate(std::string date){
    auto const regex = std::regex("[0-9]{4}(/|-|.)(0[1-9]|1[0-2])(/|-|.)([0-2][1-9]|3[0-1])(\\s([0-1][1-9]|2[0-3])(:([0-5][0-9])){2}){0,1}");
        
    return std::regex_match(date, regex);
}

rete::WME::Ptr DateDiffNode::process(rete::Token::Ptr token)
{
    std::string date1, date2;
    date1_.interpretation->getValue(token, date1);
    date2_.interpretation->getValue(token, date2);

    if(!validate(date1) || !validate(date2))
        return nullptr;

    //Once validated these substrings are fixed
    int year1 = stoi(date1.substr(0,4));
    int year2 = stoi(date2.substr(0,4));

    int month1 = stoi(date1.substr(5,2));
    int month2 = stoi(date2.substr(5,2));

    int day1 = stoi(date1.substr(8,2));
    int day2 = stoi(date2.substr(8,2));

    int dayDiff = 0;

    //Calculate number of days
    struct std::tm a = {0,0,0,day1,month1,year1-1900};
    struct std::tm b = {0,0,0,day2,month2,year2-1900};
    std::time_t t1 = std::mktime(&a);
    std::time_t t2 = std::mktime(&b);
    if ( t1 != (std::time_t)(-1) && t2 != (std::time_t)(-1) )
    {
        dayDiff = std::difftime(t2, t1) / (60 * 60 * 24); //Adjust to days
    }

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
