#ifndef SEMPR_TRIPLEDOCUMENTFILENAMEBUILDER_HPP_
#define SEMPR_TRIPLEDOCUMENTFILENAMEBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

namespace sempr {

class TripleDocumentFilenameBuilder : public rete::NodeBuilder {
public:
    TripleDocumentFilenameBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};

}


#endif /* include guard: SEMPR_TRIPLEDOCUMENTFILENAMEBUILDER_HPP_ */
