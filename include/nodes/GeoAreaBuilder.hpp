#ifndef SEMPR_GEOAREABUILDER_HPP_
#define SEMPR_GEOAREABUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

namespace sempr {

class GeoAreaBuilder : public rete::NodeBuilder {
public:
    GeoAreaBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_GEOAREABUILDER_HPP_ */
