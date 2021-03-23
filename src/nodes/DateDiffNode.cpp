#include "nodes/DateDiffNode.hpp"
#include <rete-core/TupleWME.hpp>
#include <string>
#include <cmath>

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
    date1_.interpretation->getValue(token, date1);
    date2_.interpretation->getValue(token, date2);

    int year1 = stoi(date1.substr(0,4));
    int year2 = stoi(date2.substr(0,4));

    int month1 = stoi(date1.substr(5,2));
    int month2 = stoi(date2.substr(5,2));

    int day1 = stoi(date1.substr(8,2));
    int day2 = stoi(date2.substr(8,2));

    //Calculate number of days since arbitrary starting point: e.g. 2020-01-01
    int aYear = 2020;
    int aMonth = 01;
    int aDay = 01;
    int nDays1 = (year1 - aYear)*365 + day1 - aDay;
    int nDays2 = (year2 - aYear)*365 + day2 - aDay;

    //Get the number of days for each month passed
    for(int i=month1-1; i>=aMonth; i--) nDays1 += monthLength[i-1];
    for(int i=month2-1; i>=aMonth; i--) nDays2 += monthLength[i-1];
    
    //Leap year adjustment
    if(month1 >= 2 || year1 > aYear) nDays1 += 1; //Leap year 2020
    nDays1 += (year1 - aYear)/4; //+1 day for every leap year after 2020
    
    if(month2 >= 2 || year2 > aYear) nDays2 += 1; //Leap year 2020
    nDays2 += (year2 - aYear)/4; //+1 day for every leap year after 2020

    //Get difference in days
    int dayDiff = nDays1 - nDays2;

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
