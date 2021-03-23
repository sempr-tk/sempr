#ifndef SEMPR_YEARNODE_HPP_
#define SEMPR_YEARNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

namespace sempr {

/**
    Manipulate fields from a date string, format: YYYY-MM-DD HH:MM:SS

    date:year extracts the year from a date string
*/
class DateYearNode : public rete::Builtin {
public:
    DateYearNode(rete::PersistentInterpretation<std::string> date);
    DateYearNode();

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<std::string> date_;    
};

}

#endif
