#ifndef SEMPR_DATEDIFFBUILDER_HPP_
#define SEMPR_DATEDIFFBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

namespace sempr {

class DateDiffBuilder : public rete::NodeBuilder {
public:
    DateDiffBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif
