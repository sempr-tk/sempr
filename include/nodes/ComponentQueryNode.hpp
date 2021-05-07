#ifndef SEMPR_COMPONENTQUERYNODE_HPP_
#define SEMPR_COMPONENTQUERYNODE_HPP_

#include <map>
#include <string>

#include <rete-core/Production.hpp>
#include <rete-core/Accessors.hpp>

#include "SopranoModule.hpp"

#include "../Component.hpp"
#include "../ECWME.hpp"

namespace sempr {

class ComponentQueryModule;



/**
    The ComponentQueryModule is an extension to the old SopranoModule.
    It forwards the SPARQL part of a given query to soprano, and post processes
    the result by finding the requested entities and components.

    Actually, this class only holds registered ECWME values and gives
    the ComponentQuery-classes access to them, which in turn implement the
    actual search.
*/
class ComponentQueryModule {
    // mapping: entity id to set of ECWMEs
    std::map<std::string, std::set<ECWME::Ptr>> ecwmes_;

public:
    using Ptr = std::shared_ptr<ComponentQueryModule>;

    void addECWME(ECWME::Ptr wme);
    void removeECWME(ECWME::Ptr wme);

    std::set<ECWME::Ptr> get(const std::string& id);
};


/**
    The ComponentQueryNode simply connects the rete network with the
    ComponentQueryModule, to keep track of all (asserted and inferred)
    ECWMEs.

    It assumes to have only one use case, in one very specific rule:

        [EC<Component>(?e ?c ?t) -> UpdateComponentQueryModule()]

    It does not even take arguments, but instead **assumes** that the first and
    only WME in the given Token is an ECWME!

    **DON'T USE THIS YOURSELF, IT'S HANDLED BY THE RDFPLUGIN**
*/
class ComponentQueryNode : public rete::Production {
    ComponentQueryModule::Ptr module_;
public:
    using Ptr = std::shared_ptr<ComponentQueryNode>;
    ComponentQueryNode(ComponentQueryModule::Ptr);

    void execute(rete::Token::Ptr, rete::PropagationFlag, std::vector<rete::WME::Ptr>&) override;
    std::string toString() const override;
};


}

#endif /* include guard: SEMPR_COMPONENTQUERYNODE_HPP_ */
