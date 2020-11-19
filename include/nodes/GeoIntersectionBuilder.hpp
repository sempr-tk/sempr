#ifndef SEMPR_GEOINTERSECTIONBUILDER_HPP_
#define SEMPR_GEOINTERSECTIONBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

namespace sempr {

class GeoIntersectionBuilder : public rete::NodeBuilder {
public:
    GeoIntersectionBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_GEOINTERSECTIONBUILDER_HPP_ */
