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
    rete::util::requireNumberOfArgs(args, 2);
    rete::util::requireUnboundVariable(args, 0);

    // note: the construction can not happen as easy as usual, as the
    // CreateEntityNode supports both std::string and rete::TriplePart
    // arguments -- string-ids and URIs -- and handles them differently.
    rete::Builtin::Ptr node;
    if (args[1].isConst())
    {
        if (args[1].getAST().isURI())
        {
            // requireInterpretation<rete::TriplePart> is not implemented
            // for args with arg.isConst()
            rete::ConstantAccessor<rete::TriplePart> acc({args[1].getAST().toString()});
            acc.index() = 0;
            node = std::make_shared<CreateEntityNode>(
                    acc.getInterpretation<rete::TriplePart>()->makePersistent()
            );
        }
        else
        {
            node = std::make_shared<CreateEntityNode>(
                    rete::util::requireInterpretation<std::string>(args, 1)
            );
        }
    }
    else /* isVariable */
    {
        try {
            // try a triple-part interpretation -> URI
            auto uri = rete::util::requireInterpretation<rete::TriplePart>(args, 1);
            node = std::make_shared<CreateEntityNode>(std::move(uri));
        } catch (rete::NodeBuilderException&) {
            // try a string interpretation
            auto id = rete::util::requireInterpretation<std::string>(args, 1);
            node = std::make_shared<CreateEntityNode>(std::move(id));
        }
    }

    auto resultAccessor = std::make_shared<rete::TupleWME<Entity::Ptr>::Accessor<0>>();
    args[0].bind(resultAccessor);
    return node;
}

}
