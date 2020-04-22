#ifndef SEMPR_INFERECNODE_HPP_
#define SEMPR_INFERECNODE_HPP_

#include <rete-core/Production.hpp>
#include <rete-core/Accessors.hpp>

#include "../ECAccessors.hpp"
#include "../ECWME.hpp"

namespace sempr {

/**
    A rule effect to infer a connection between an entity and a component. This
    can be used to add components that are computed as part of a rule to the
    reasoner.

    This does **not** add the component to the entities component-list. It only
    infers a ECWME as if it had been. This also means that the component does
    not have access to the entity -- adding e.g. a TriplePropertyMap will not
    work as intended. Use this only for components that do not require access
    to their entity (e.g. for the id).
*/
class InferECNode : public rete::Production {
    /*
        Note: Better would be a SpecificTypeAccessor here, too. But since I added
        the StringAccessor base to the EntityAccessor (for easy interpretation
        as a string, for more intuitive rules) this is not possible anymore.
        Changing the EntityAccessor was a quick hack. The whole
        accessor-structure needs a rework that removes those pesky inheritance
        hierarchy and replaces it with... components/capabilities...
        I notice a pattern... ;)
    */
    std::unique_ptr<EntityAccessor> entity_;
    std::unique_ptr<rete::SpecificTypeAccessor<Component::Ptr>> component_;
public:
    using Ptr = std::shared_ptr<InferECNode>;
    InferECNode(
        std::unique_ptr<EntityAccessor> entity,
        std::unique_ptr<rete::SpecificTypeAccessor<Component::Ptr>> component
    );

    void execute(rete::Token::Ptr, rete::PropagationFlag, std::vector<rete::WME::Ptr>&) override;

    std::string toString() const override;
};


}

#endif /* include guard: SEMPR_INFERECNODE_HPP_ */
