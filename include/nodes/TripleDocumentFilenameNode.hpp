#ifndef SEMPR_TRIPLEDOCUMENTFILENAMENODE_HPP_
#define SEMPR_TRIPLEDOCUMENTFILENAMENODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Util.hpp>
#include <rete-core/Accessors.hpp>

#include "../component/TripleDocument.hpp"

namespace sempr {

/**
    This node extracts the filename of a TripleDocument component.
    Returns a TupleWME<std::string> with the filename.
*/
class TripleDocumentFilenameNode : public rete::Builtin {
    std::unique_ptr<rete::SpecificTypeAccessor<TripleDocument::Ptr>> document_;
public:
    TripleDocumentFilenameNode(std::unique_ptr<rete::SpecificTypeAccessor<TripleDocument::Ptr>>);

    rete::WME::Ptr process(rete::Token::Ptr) override;

    bool operator == (const rete::BetaNode& other) const override;
};


}

#endif /* include guard: SEMPR_TRIPLEDOCUMENTFILENAMENODE_HPP_ */
