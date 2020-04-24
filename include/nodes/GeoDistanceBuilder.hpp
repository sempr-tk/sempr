#ifndef SEMPR_GEODISTANCEBUILDER_HPP_
#define SEMPR_GEODISTANCEBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

namespace sempr {

class GeoDistanceBuilder : public rete::NodeBuilder {
public:
    GeoDistanceBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_GEODISTANCEBUILDER_HPP_ */
