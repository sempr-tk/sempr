#ifndef SEMPR_EXTRACTTRIPLES_HPP_
#define SEMPR_EXTRACTTRIPLES_HPP_

#include <rete-core/Production.hpp>
#include <rete-core/Accessors.hpp>
#include <rete-rdf/Triple.hpp>

#include "../ECAccessors.hpp"
#include "../component/TripleContainer.hpp"

namespace sempr {

/**
    The ExtractTriples effect node. Takes a TripleContainer and infers
    the triples that it contains as individual WMEs.
*/
class ExtractTriples : public rete::Production {
    rete::PersistentInterpretation<TripleContainer::Ptr> container_;
public:
    using Ptr = std::shared_ptr<ExtractTriples>;
    ExtractTriples(rete::PersistentInterpretation<TripleContainer::Ptr> container);

    /**
        Infer triples from the TripleContainer in the token, iff the propagationflag
        is ASSERT or UPDATE. In case of UPDATE, the old data is retracted
        automatically by the reasoner, so just handle it as if it were an
        ASSERT.
    */
    void execute(rete::Token::Ptr, rete::PropagationFlag, std::vector<rete::WME::Ptr>&) override;


    /**
        For visualization
    */
    std::string toString() const override;
};


}

#endif /* include guard : SEMPR_EXTRACTTRIPLES_HPP_ */
