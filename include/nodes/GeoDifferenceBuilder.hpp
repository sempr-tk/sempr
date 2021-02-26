#ifndef SEMPR_GEODIFFERENCEBUILDER_HPP_
#define SEMPR_GEODIFFERENCEBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

namespace sempr {

class GeoDifferenceBuilder : public rete::NodeBuilder {
public:
    GeoDifferenceBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_GEODIFFERENCEBUILDER_HPP_ */
