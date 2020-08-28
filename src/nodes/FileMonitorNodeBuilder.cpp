#include <rete-reasoner/Exceptions.hpp>
#include "nodes/FileMonitorNodeBuilder.hpp"

namespace sempr {

FileMonitorNodeBuilder::FileMonitorNodeBuilder(std::recursive_mutex& mutex)
    : rete::NodeBuilder("file:exists", rete::NodeBuilder::BuilderType::BUILTIN),
      mutex_(mutex)
{
}

rete::Builtin::Ptr FileMonitorNodeBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // this node produces no relevant output (EmptyWME), and thus only needs
    // exactly one string argument, which is the filename.
    if (args.size() != 1) throw rete::NodeBuilderException("Invalid number of args (!= 1)");
    if (args[0].isVariable() && !args[0].getAccessor()) throw rete::NodeBuilderException("Argument  " + args[0].getVariableName() + " must not be unbound.");

    rete::PersistentInterpretation<std::string> interpretation;
    if (args[0].isConst())
    {
        auto acc = new rete::ConstantAccessor<std::string>(args[0].getAST());
        acc->index() = 0; // allow accessor to be applied to a token
        interpretation = acc->getInterpretation<std::string>()->makePersistent();
    }
    else
    {
        auto in = args[0].getAccessor()->getInterpretation<std::string>();
        if (!in)
            throw rete::NodeBuilderException("Argument " + args[0].getVariableName() + " not compatible to StringAccessor.");

        interpretation = in->makePersistent();
    }

    // create the node:;
    auto node = std::make_shared<FileMonitorNode>(mutex_, std::move(interpretation));
    return node;
}


}
