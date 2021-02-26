#ifndef SEMPR_GEOFROMWKTBUILDER_HPP_
#define SEMPR_GEOFROMWKTBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

namespace sempr {

class GeoFromWKTBuilder : public rete::NodeBuilder {
public:
    GeoFromWKTBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_GEOFROMWKTBUILDER_HPP_ */
