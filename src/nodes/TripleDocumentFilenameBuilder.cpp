#include <rete-reasoner/Exceptions.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/TupleWMEAccessor.hpp>

#include "nodes/TripleDocumentFilenameBuilder.hpp"
#include "nodes/TripleDocumentFilenameNode.hpp"

namespace sempr {

TripleDocumentFilenameBuilder::TripleDocumentFilenameBuilder()
    : rete::NodeBuilder("td:filename", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr TripleDocumentFilenameBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // there must be exactly 2 arguments: Unbound variable for the result, and
    // a variable bound to a TripleDocument.
    if (args.size() != 2) throw rete::NodeBuilderException("Invalid number of arguments (!= 2).");
    if (!args[0].isVariable() || args[0].getAccessor()) throw rete::NodeBuilderException("First argument must be ab unbound variable for the result.");

    if (!args[1].isVariable() || !args[1].getAccessor() ||
        !args[1].getAccessor()->canAs<rete::SpecificTypeAccessor<TripleDocument::Ptr>>())
    {
        throw rete::NodeBuilderException("Second argument must be bound to a TripleDocument.");
    }

    // clone the accessor
    std::unique_ptr<rete::SpecificTypeAccessor<TripleDocument::Ptr>>
    doc(
        args[1].getAccessor()->clone()->as<rete::SpecificTypeAccessor<TripleDocument::Ptr>>()
    );

    // create the node
    auto node = std::make_shared<TripleDocumentFilenameNode>(std::move(doc));

    // create an accessor for the result
    auto resultAcc = std::make_shared<rete::TupleWMEAccessor<0, rete::TupleWME<std::string>>>();

    // bind the result accessor
    args[0].bind(resultAcc);

    // return the node
    return node;
}

}
