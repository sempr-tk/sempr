#ifndef SEMPR_EXTRACTTRIPLESBUILDER_HPP_
#define SEMPR_EXTRACTTRIPLESBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

#include "ExtractTriples.hpp"
#include "Utility.hpp"

namespace sempr {

/**
    Constructs the "ExtractTriples" node for rules with the effect:
        [... -> ExtractTriples(?container)]
*/
class ExtractTriplesBuilder : public rete::NodeBuilder {
public:
    ExtractTriplesBuilder();
    rete::Production::Ptr buildEffect(rete::ArgumentList& args) const override;
};

}

#endif /* include guard: SEMPR_EXTRACTTRIPLESBUILDER_HPP_ */
