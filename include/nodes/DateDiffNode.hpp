#ifndef SEMPR_DATEDIFFNODE_HPP_
#define SEMPR_DATEDIFFNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

namespace sempr {

/*
    Compute the difference in days between two date strings, format: YYYY-MM-DD HH:MM:SS    
*/
class DateDiffNode : public rete::Builtin {
public:
    DateDiffNode(rete::PersistentInterpretation<std::string> date1, rete::PersistentInterpretation<std::string> date2);
    DateDiffNode();

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<std::string> date1_;
    rete::PersistentInterpretation<std::string> date2_;
    int monthLength [12] = {31,28,31,30,31,30,31,31,30,31,30,31};
};

}

#endif
