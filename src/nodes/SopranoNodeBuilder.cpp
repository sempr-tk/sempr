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
        rete::PersistentInterpretation<std::string>& acc) const
{
    if (arg.isConst())
    {
        rete::ConstantAccessor<std::string> accessor(arg.getAST());
        acc = accessor.getInterpretation<std::string>()->makePersistent();
    }
    else
    {
        if (arg.getAccessor()->getInterpretation<std::string>())
        {
            acc = arg.getAccessor()->getInterpretation<std::string>()->makePersistent();
        }
        else
        {
            throw rete::NodeBuilderException("Argument " + arg.getVariableName()
                                             + " has no string interpretation.");
        }
    }
}

rete::Production::Ptr SopranoNodeBuilder::buildEffect(rete::ArgumentList& args) const
{
    // module_ must be set!
    if (!module_) throw rete::NodeBuilderException("No SopranoModule set in SopranoNodeBuilder!");

    // must have exactly 3 arguments, which are string accessors
    if (args.size() != 3) throw rete::NodeBuilderException("Wrong number of arguments (!=3)");

    rete::PersistentInterpretation<std::string> s, p, o;
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
