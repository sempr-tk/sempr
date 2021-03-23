#include "nodes/DateYearNode.hpp"
#include <rete-core/TupleWME.hpp>
#include <string>

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

    //std::cout << "date:Year | token = " << date << std::endl;

    int year = stoi(date.substr(0,4)); //Year = 4 initial characters

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
