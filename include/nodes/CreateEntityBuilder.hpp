#ifndef SEMPR_CREATEENTITYBUILDER_HPP_
#define SEMPR_CREATEENTITYBUILDER_HPP_

#include <rete-reasoner/NodeBuilder.hpp>

namespace sempr {

/**
    Builds the createEntity(?entity ?id) nodes, which create temporary entities
    with the given id.
*/
class CreateEntityBuilder : public rete::NodeBuilder {
public:
    CreateEntityBuilder();
    rete::Builtin::Ptr buildBuiltin(rete::ArgumentList& args) const override;
};


}

#endif /* include guard: SEMPR_CREATEENTITYBUILDER_HPP_ */
