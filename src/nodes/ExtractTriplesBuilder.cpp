#include "nodes/ExtractTriplesBuilder.hpp"

namespace sempr {

ExtractTriplesBuilder::ExtractTriplesBuilder()
    : rete::NodeBuilder(
            "ExtractTriples",
            rete::NodeBuilder::BuilderType::EFFECT)
{
}

rete::Production::Ptr ExtractTriplesBuilder::buildEffect(rete::ArgumentList& args) const
{
    // must have exactly one argument,
    // and it has to be a TripleContainer-Accessor
    if (args.size() != 1) throw rete::NodeBuilderException("Wrong number of arguments (!=1)");
    if (args[0].isConst()) throw rete::NodeBuilderException("Argument must be a variable bound to a TripleContainer, but is a constant");

    auto accessor = args[0].getAccessor();
    auto containerAccessor = std::dynamic_pointer_cast<ComponentAccessor<TripleContainer>>(accessor);

    if (!containerAccessor) throw rete::NodeBuilderException("Argument must be a TripleContainer");

    // clone the accessor
    std::unique_ptr<ComponentAccessor<TripleContainer>> acc(containerAccessor->clone());

    // create the node
    ExtractTriples::Ptr node(new ExtractTriples(std::move(acc)));
    return node;
}

}
