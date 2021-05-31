#ifndef SEMPR_DATEYEARBUILDER_HPP_
#define SEMPR_DATEYEARBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

namespace sempr {

class DateYearBuilder : public rete::NodeBuilder {
public:
    DateYearBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif
