#ifndef SEMPR_DATEBUILDER_HPP_
#define SEMPR_DATEBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

namespace sempr {

/*class Date : public rete::NodeBuilder {
public:
    Date();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};*/

class DateMonthBuilder : public rete::NodeBuilder {
public:
    DateMonthBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif
