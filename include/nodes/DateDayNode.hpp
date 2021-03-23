#ifndef SEMPR_DATEDAYNODE_HPP_
#define SEMPR_DATEDAYNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

namespace sempr {

/**
    Manipulate fields from a date string, format: YYYY-MM-DD HH:MM:SS

    date:day extracts the day from a date string
*/
class DateDayNode : public rete::Builtin {
public:
    DateDayNode(rete::PersistentInterpretation<std::string> date);
    DateDayNode();

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<std::string> date_;    
};

}

#endif
