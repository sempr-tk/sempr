#ifndef SEMPR_DATEPLUSNODE_HPP_
#define SEMPR_DATEPLUSNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

namespace sempr {

/*
    Compute the new date resulting from adding n days to another date, format: YYYY-MM-DD HH:MM:SS
*/
class DatePlusNode : public rete::Builtin {
public:
    DatePlusNode(rete::PersistentInterpretation<std::string> date, rete::PersistentInterpretation<int> days);

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<std::string> date_;
    rete::PersistentInterpretation<int> days_;
};

}

#endif
