#ifndef SEMPR_DATENODE_HPP_
#define SEMPR_DATENODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

namespace sempr {

/**
    Manipulate fields from a date string, format: YYYY-MM-DD HH:MM:SS

class DateNode : public rete::Builtin {
public:
    DateNode(rete::PersistentInterpretation<std::string> date);
    DateNode();

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<std::string> date_;    
};*/

/*
    Extract month from date string
*/
class DateMonthNode : public rete::Builtin {
public:
    DateMonthNode(rete::PersistentInterpretation<std::string> date);
    DateMonthNode();

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<std::string> date_;    
};

}

#endif
