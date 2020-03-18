#ifndef SEMPR_LOADTRIPLESFROMFILENODE_HPP_
#define SEMPR_LOADTRIPLESFROMFILENODE_HPP_

#include <rete-core/Production.hpp>
#include <rete-core/Accessors.hpp>
#include <rete-rdf/Triple.hpp>

namespace sempr {

/**
    The LoadTriplesFromFileNode does exactly what the name says: It interprets a
    given string as a filename, tries to read an parse the file, and returns the
    triples contained therein.

    Supported formats:
    - RDFXML
    - Turtle
    - NTriples
*/
class LoadTriplesFromFileNode : public rete::Production {
    std::unique_ptr<rete::StringAccessor> filename_;
public:
    using Ptr = std::shared_ptr<LoadTriplesFromFileNode>;
    LoadTriplesFromFileNode(std::unique_ptr<rete::StringAccessor> filename);

    /**
        Infers triples from the given Token by extracting a filename from it,
        from which it loads the triples.
    */
    void execute(rete::Token::Ptr, rete::PropagationFlag, std::vector<rete::WME::Ptr>&) override;

    std::string toString() const override;
};

}

#endif /* include guard: SEMPR_LOADTRIPLESFROMFILENODE_HPP_ */
