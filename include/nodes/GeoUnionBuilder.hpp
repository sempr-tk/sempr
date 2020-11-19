#ifndef SEMPR_GEOUNIONBUILDER_HPP_
#define SEMPR_GEOUNIONBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

namespace sempr {

class GeoUnionBuilder : public rete::NodeBuilder {
public:
    GeoUnionBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_GEOUNIONBUILDER_HPP_ */
