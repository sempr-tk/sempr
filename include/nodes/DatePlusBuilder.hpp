#ifndef SEMPR_DATEPLUSBUILDER_HPP_
#define SEMPR_DATEPLUSBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

namespace sempr {

class DatePlusBuilder : public rete::NodeBuilder {
public:
    DatePlusBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif
