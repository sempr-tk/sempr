#ifndef SEMPR_CREATEENTITYNODE_HPP_
#define SEMPR_CREATEENTITYNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Util.hpp>
#include <rete-core/Accessors.hpp>
#include <rete-rdf/TriplePart.hpp>

#include "../Entity.hpp"

namespace sempr {


/**
    The CreateEntityNode is a simple utility which wraps an Entity around a
    given ID. This is needed to infer more complex structures from rules:
    Imagine you already inferred the existence of a new semantic entity, which
    you named using the makeSkolem builtin. Now you want to assign a computed
    geometry to it -- but to use the InferECNode, you need an actual Entity,
    not just a string-id! --> Use this node to create a temporary entity with
    the given id.

TODO: - global ID pool, to properly reuse existing entities?
      - use sempr::Core/IDGenerationStrategy? To check if such an entity
        already exists?
*/
class CreateEntityNode : public rete::Builtin {
    rete::PersistentInterpretation<std::string> id_;
    rete::PersistentInterpretation<rete::TriplePart> uri_;
public:
    CreateEntityNode(rete::PersistentInterpretation<std::string> id);
    CreateEntityNode(rete::PersistentInterpretation<rete::TriplePart> uri);

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;
};

}


#endif /* include guard: SEMPR_CREATEENTITYNODE_HPP_ */
