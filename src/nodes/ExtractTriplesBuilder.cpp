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
    rete::util::requireNumberOfArgs(args, 1);
    // and it has to be a TripleContainer-Accessor
    auto inter = rete::util::requireInterpretation<TripleContainer::Ptr>(args, 0);

    ExtractTriples::Ptr node(new ExtractTriples(std::move(inter)));
    return node;
}

}
