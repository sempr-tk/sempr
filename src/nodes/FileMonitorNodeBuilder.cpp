#include <rete-reasoner/Exceptions.hpp>
#include "nodes/FileMonitorNodeBuilder.hpp"

namespace sempr {

FileMonitorNodeBuilder::FileMonitorNodeBuilder(std::mutex& mutex)
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

    std::unique_ptr<rete::StringAccessor> acc;
    if (args[0].isConst())
    {
        acc.reset(new rete::ConstantStringAccessor(args[0].getAST()));
    }
    else
    {
        if(!args[0].getAccessor()->canAs<rete::StringAccessor>())
            throw rete::NodeBuilderException("Argument " + args[0].getVariableName() + " not compatible to StringAccessor.");

        acc.reset(args[0].getAccessor()->clone()->as<rete::StringAccessor>());
    }

    // create the node:;
    auto node = std::make_shared<FileMonitorNode>(mutex_, std::move(acc));
    return node;
}


}
