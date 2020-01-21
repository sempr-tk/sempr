#include <rete-core/Accessors.hpp>

#include "nodes/SopranoNodeBuilder.hpp"

namespace sempr {

SopranoNodeBuilder::SopranoNodeBuilder(SopranoModule::Ptr module)
    : rete::NodeBuilder(
            "SopranoModule",
            rete::NodeBuilder::BuilderType::EFFECT),
      module_(module)
{
}

void SopranoNodeBuilder::argToAccessor(
        rete::Argument& arg,
        std::unique_ptr<rete::StringAccessor>& acc) const
{
    if (arg.isConst())
    {
        acc.reset(new rete::ConstantStringAccessor(arg.getAST()));
    }
    else
    {
        if (arg.getAccessor()->canAs<rete::StringAccessor>())
        {
            acc.reset(arg.getAccessor()->clone()->as<rete::StringAccessor>());
        }
        else
        {
            throw rete::NodeBuilderException("Argument " + arg.getVariableName()
                                             + " is not compatible to string");
        }
    }
}

rete::Production::Ptr SopranoNodeBuilder::buildEffect(rete::ArgumentList& args) const
{
    // module_ must be set!
    if (!module_) throw rete::NodeBuilderException("No SopranoModule set in SopranoNodeBuilder!");

    // must have exactly 3 arguments, which are string accessors
    if (args.size() != 3) throw rete::NodeBuilderException("Wrong number of arguments (!=3)");

    std::unique_ptr<rete::StringAccessor> s, p, o;
    argToAccessor(args[0], s);
    argToAccessor(args[1], p);
    argToAccessor(args[2], o);

    auto node = std::make_shared<SopranoNode>(module_,
                                              std::move(s),
                                              std::move(p),
                                              std::move(o));
    return node;
}


}
