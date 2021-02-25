#include <rete-core/Accessors.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/TupleWMEAccessor.hpp>

#include "TupleEntityAccessor.hpp"
#include "nodes/CreateEntityNode.hpp"
#include "nodes/CreateEntityBuilder.hpp"

namespace sempr {

CreateEntityBuilder::CreateEntityBuilder()
    : rete::NodeBuilder("createEntity", rete::NodeBuilder::BuilderType::BUILTIN)
{
}


rete::Builtin::Ptr CreateEntityBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // need exactly 2 arguments: Unbound variable for the result, and a string
    // for the id
    if (args.size() != 2)
        throw rete::NodeBuilderException("Invalid number of arguments (!= 2)");
    if (args[0].isConst() || args[0].getAccessor())
        throw rete::NodeBuilderException("First argument must be unbound");

    rete::Builtin::Ptr node;
    if (args[1].isConst())
    {
        rete::ConstantAccessor<std::string> acc(args[1].getAST().toString());
        acc.index() = 0;
        node = std::make_shared<CreateEntityNode>(
                acc.getInterpretation<std::string>()->makePersistent());
    }
    else /* isVariable */
    {
        if (args[1].getAccessor())
        {
            // try a triple-part interpretation -> URI
            auto tpI = args[1].getAccessor()->getInterpretation<rete::TriplePart>();
            if (tpI)
                node =std::make_shared<CreateEntityNode>(tpI->makePersistent());
            else
            {
                // try a string interpretation
                auto strI = args[1].getAccessor()->getInterpretation<std::string>();
                if (strI)
                    node = std::make_shared<CreateEntityNode>(strI->makePersistent());
                else
                    throw rete::NodeBuilderException(
                            args[1].getVariableName() +
                            " has an invalid type for the entities ID");
            }
        }
        else
        {
            throw rete::NodeBuilderException("Second argument cannot be unbound");
        }
    }

    auto resultAccessor = std::make_shared<rete::TupleWME<Entity::Ptr>::Accessor<0>>();
    args[0].bind(resultAccessor);
    return node;
}

}
