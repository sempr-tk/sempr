#include "nodes/LoadTriplesFromFileBuilder.hpp"
#include "nodes/LoadTriplesFromFileNode.hpp"

#include <rete-reasoner/Exceptions.hpp>

#include <iostream>

namespace sempr {

LoadTriplesFromFileBuilder::LoadTriplesFromFileBuilder()
    : rete::NodeBuilder(
            "LoadTriplesFromFile",
            rete::NodeBuilder::BuilderType::EFFECT)
{
}

rete::Production::Ptr LoadTriplesFromFileBuilder::buildEffect(rete::ArgumentList& args) const
{
    // must have exactly one argument, namely a string (const or accessor)
    if (args.size() != 1) throw rete::NodeBuilderException("Wrong number of arguments (!= 1).");

    std::cout << "args.size(): " << args.size() << std::endl;
    std::cout << "args[0].isConst(): " << args[0].isConst() << std::endl;
    std::cout << "args[0].getAccessor(): " << args[0].getAccessor() << std::endl;


    // create the accessor
    std::unique_ptr<rete::StringAccessor> filename;
    if (args[0].isConst())
    {
        // TODO: Due to a bug in rete this includes quotation marks :(
        filename.reset(new rete::ConstantStringAccessor(args[0].getAST()));
    }
    else
    {
        if (!args[0].getAccessor()->canAs<rete::StringAccessor>())
            throw rete::NodeBuilderException("Argument " + args[0].getVariableName() + " is not compatible with StringAccessor.");
        filename.reset(args[0].getAccessor()->clone()->as<rete::StringAccessor>());
    }

    // build the node
    auto node = std::make_shared<LoadTriplesFromFileNode>(std::move(filename));
    return node;
}


}
