#include <rete-core/TupleWME.hpp>

#include "nodes/TripleDocumentFilenameNode.hpp"

namespace sempr {

TripleDocumentFilenameNode::TripleDocumentFilenameNode(
        rete::PersistentInterpretation<TripleDocument::Ptr> doc)
    : rete::Builtin("td:filename"),
      document_(std::move(doc))
{
}

rete::WME::Ptr TripleDocumentFilenameNode::process(rete::Token::Ptr token)
{
    TripleDocument::Ptr doc;
    document_.interpretation->getValue(token, doc);

    auto result = std::make_shared<rete::TupleWME<std::string>>(doc->getFile());
    return result;
}

bool TripleDocumentFilenameNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const TripleDocumentFilenameNode*>(&other);
    if (!o) return false;
    return *(o->document_.accessor) == *(this->document_.accessor);
}

}
