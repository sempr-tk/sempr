#ifndef SEMPR_DATENODE_HPP_
#define SEMPR_DATENODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

namespace sempr {

/*
    Extract month from date string
*/
class DateMonthNode : public rete::Builtin {
public:
    DateMonthNode(rete::PersistentInterpretation<std::string> date);

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<std::string> date_;    
};

}

#endif
