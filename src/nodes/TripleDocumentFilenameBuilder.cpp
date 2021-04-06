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
    rete::util::requireNumberOfArgs(args, 2);
    rete::util::requireUnboundVariable(args, 0);

    auto doc = rete::util::requireInterpretation<TripleDocument::Ptr>(args, 1);

    // create the node
    auto node = std::make_shared<TripleDocumentFilenameNode>(std::move(doc));

    // create an accessor for the result
    auto resultAcc = std::make_shared<rete::TupleWME<std::string>::Accessor<0>>();

    // bind the result accessor
    args[0].bind(resultAcc);

    // return the node
    return node;
}

}
