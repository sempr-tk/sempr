#ifndef SEMPR_GEOTOWKTBUILDER_HPP_
#define SEMPR_GEOTOWKTBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

namespace sempr {

class GeoToWKTBuilder : public rete::NodeBuilder {
public:
    GeoToWKTBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_GEOTOWKTBUILDER_HPP_ */
