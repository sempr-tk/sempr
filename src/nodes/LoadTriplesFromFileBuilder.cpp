#include "nodes/LoadTriplesFromFileBuilder.hpp"
#include "nodes/LoadTriplesFromFileNode.hpp"

#include <rete-reasoner/Exceptions.hpp>

#include <iostream>

namespace sempr {

LoadTriplesFromFileBuilder::LoadTriplesFromFileBuilder()
    : rete::NodeBuilder(
            "LoadTriplesFromFile",
            rete::NodeBuilder::BuilderType::EFFECT)
{
}

rete::Production::Ptr LoadTriplesFromFileBuilder::buildEffect(rete::ArgumentList& args) const
{
    // must have exactly one argument, namely a string (const or accessor)
    rete::util::requireNumberOfArgs(args, 1);
    auto filename = rete::util::requireInterpretation<std::string>(args, 0);

    // build the node
    auto node = std::make_shared<LoadTriplesFromFileNode>(std::move(filename));
    return node;
}


}
