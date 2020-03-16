#ifndef SEMPR_LOADTRIPLESFROMFILEBUILDER_HPP_
#define SEMPR_LOADTRIPLESFROMFILEBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

#include "nodes/LoadTriplesFromFileNode.hpp"

namespace sempr {

/**
    Constructs the LoadTriplesFromFile-Nodes.
*/
class LoadTriplesFromFileBuilder : public rete::NodeBuilder {
public:
    LoadTriplesFromFileBuilder();
    rete::Production::Ptr buildEffect(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_LOADTRIPLESFROMFILEBUILDER_HPP_ */
