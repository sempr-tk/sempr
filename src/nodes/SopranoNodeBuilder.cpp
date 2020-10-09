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
        rete::PersistentInterpretation<rete::TriplePart>& acc) const
{
    if (arg.isConst())
    {
        rete::ConstantAccessor<rete::TriplePart> accessor({arg.getAST()});
        acc = accessor.getInterpretation<rete::TriplePart>()->makePersistent();
    }
    else
    {
        if (arg.getAccessor()->getInterpretation<rete::TriplePart>())
        {
            acc = arg.getAccessor()->getInterpretation<rete::TriplePart>()->makePersistent();
        }
        else
        {
            throw rete::NodeBuilderException(
                    "Argument " + arg.getVariableName() +
                    " has no interpretation as TriplePart.");
        }
    }
}

rete::Production::Ptr SopranoNodeBuilder::buildEffect(rete::ArgumentList& args) const
{
    // module_ must be set!
    if (!module_) throw rete::NodeBuilderException("No SopranoModule set in SopranoNodeBuilder!");

    // must have exactly 3 arguments, which are string accessors
    if (args.size() != 3) throw rete::NodeBuilderException("Wrong number of arguments (!=3)");

    rete::PersistentInterpretation<rete::TriplePart> s, p, o;
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
