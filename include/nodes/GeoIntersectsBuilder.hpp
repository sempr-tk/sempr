#ifndef SEMPR_GEOINTERSECTSBUILDER_HPP_
#define SEMPR_GEOINTERSECTSBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

namespace sempr {

class GeoIntersectsBuilder : public rete::NodeBuilder {
public:
    GeoIntersectsBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_GEOINTERSECTSBUILDER_HPP_ */
