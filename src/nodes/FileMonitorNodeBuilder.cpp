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
    rete::util::requireNumberOfArgs(args, 1);
    auto interpretation = rete::util::requireInterpretation<std::string>(args, 0);

    // create the node:;
    auto node = std::make_shared<FileMonitorNode>(mutex_, std::move(interpretation));
    return node;
}


}
