#ifndef SEMPR_COMPONENTQUERYNODEBUILDER_HPP_
#define SEMPR_COMPONENTQUERYNODEBUILDER_HPP_


#include <rete-reasoner/NodeBuilder.hpp>
#include <rete-reasoner/Exceptions.hpp>
#include <rete-rdf/TriplePart.hpp>

#include "ComponentQueryNode.hpp"

namespace sempr {

class ComponentQueryBuilder : public rete::NodeBuilder {
    ComponentQueryModule::Ptr module_;
public:
    ComponentQueryBuilder(ComponentQueryModule::Ptr mod);
    rete::Production::Ptr buildEffect(rete::ArgumentList& args) const override;
};


}


#endif /* include guard: SEMPR_COMPONENTQUERYNODEBUILDER_HPP_ */
