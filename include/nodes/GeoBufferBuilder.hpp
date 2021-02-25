#ifndef SEMPR_GEOBUFFERBUILDER_HPP_
#define SEMPR_GEOBUFFERBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

namespace sempr {

class GeoBufferBuilder : public rete::NodeBuilder {
public:
    GeoBufferBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_GEOBUFFERBUILDER_HPP_ */
