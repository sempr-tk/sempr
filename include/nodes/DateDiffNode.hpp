#ifndef SEMPR_DATEDIFFNODE_HPP_
#define SEMPR_DATEDIFFNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

namespace sempr {

/*
    Compute the difference in days between two date strings, format: YYYY-MM-DD HH:MM:SS

    The difference is always with respect to date1, so:
        diff > 0 -> date1 > date2
        diff < 0 -> date1 < date2
*/
class DateDiffNode : public rete::Builtin {
public:
    DateDiffNode(rete::PersistentInterpretation<std::string> date1, rete::PersistentInterpretation<std::string> date2);    

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<std::string> date1_;
    rete::PersistentInterpretation<std::string> date2_;
    bool validate(std::string);
};

}

#endif
