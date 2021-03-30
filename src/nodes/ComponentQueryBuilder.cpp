#include "nodes/ComponentQueryNode.hpp"
#include "nodes/ComponentQueryBuilder.hpp"

namespace sempr {

ComponentQueryBuilder::ComponentQueryBuilder(ComponentQueryModule::Ptr mod)
    :
        rete::NodeBuilder(
               "ComponentQueryModule",
                rete::NodeBuilder::BuilderType::EFFECT),
        module_(mod)
{
}

rete::Production::Ptr ComponentQueryBuilder::buildEffect(rete::ArgumentList& args) const
{
    if (!module_)
        throw rete::NodeBuilderException(
                "No ComponentQueryModule set in ComponentQueryBuilder");

    if (args.size() > 0)
        throw rete::NodeBuilderException(
                "ComponentQueryModule() does not take any arguments,"
                " but instead assumes the pattern yields exactly one ECWME.");

    auto node = std::make_shared<ComponentQueryNode>(module_);
    return node;
}

}
