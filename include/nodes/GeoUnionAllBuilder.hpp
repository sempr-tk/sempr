#ifndef SEMPR_GEOUNIONALLBUILDER_HPP_
#define SEMPR_GEOUNIONALLBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

namespace sempr {

class GeoUnionAllBuilder : public rete::NodeBuilder {
public:
    GeoUnionAllBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_GEOUNIONALLBUILDER_HPP_ */
