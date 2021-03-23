#ifndef SEMPR_DATEDAYBUILDER_HPP_
#define SEMPR_DATEDAYBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

namespace sempr {

class DateDayBuilder : public rete::NodeBuilder {
public:
    DateDayBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif
