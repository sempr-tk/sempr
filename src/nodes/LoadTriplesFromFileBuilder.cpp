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
    rete::PersistentInterpretation<std::string> filename;
    if (args[0].isConst())
    {
        rete::ConstantAccessor<std::string> acc(args[0].getAST());
        acc.index() = 0;
        filename = acc.getInterpretation<std::string>()->makePersistent();
    }
    else
    {
        if (!args[0].getAccessor()->getInterpretation<std::string>())
            throw rete::NodeBuilderException("Argument " + args[0].getVariableName() + " has no string interpretation.");
        filename = args[0].getAccessor()->getInterpretation<std::string>()->makePersistent();
    }

    // build the node
    auto node = std::make_shared<LoadTriplesFromFileNode>(std::move(filename));
    return node;
}


}
