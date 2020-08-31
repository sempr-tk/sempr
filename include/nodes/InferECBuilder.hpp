#ifndef SEMPR_INFERECBUILDER_HPP_
#define SEMPR_INFERECBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>

#include "InferECNode.hpp"
#include "../Utility.hpp"

namespace sempr {

/**
    Constructs the InferECNodes, which are used to make computed components
    visible to the reasoner by associating them with entities.
    (Note: The entity will *not* be visible to the component object! This is
           only for the reasoner!)
    [... -> EC(?entity ?component)]

    Setting tags is not supported, as this would enable overwriting tags (which
    are stored at the component, not in the ECWME).
*/
class InferECBuilder : public rete::NodeBuilder {
public:
    InferECBuilder();
    rete::Production::Ptr buildEffect(rete::ArgumentList& args) const override;
};


}

#endif /* include guard: SEMPR_INFERECBUILDER_HPP_ */

