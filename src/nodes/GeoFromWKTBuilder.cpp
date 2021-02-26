#include "nodes/GeoFromWKTBuilder.hpp"
#include "nodes/GeoFromWKTNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/builtins/NumberToNumberConversion.hpp>

namespace sempr {


GeoFromWKTBuilder::GeoFromWKTBuilder()
    : rete::NodeBuilder("geo:fromWKT", rete::NodeBuilder::BuilderType::BUILTIN)
{
}


rete::Builtin::Ptr GeoFromWKTBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 2 args: result, wkt
    if (args.size() != 2) throw rete::NodeBuilderException("Invalid number of arguments (!=2)");
    if (args[0].isConst() || args[0].getAccessor()) throw rete::NodeBuilderException("First argument must be unbound, as it is reserved for the result.");

    rete::PersistentInterpretation<std::string> wkt;
    if (args[1].isConst())
    {
        if (args[1].getAST().isString())
        {
            rete::ConstantAccessor<std::string> acc(args[1].getAST().toString());
            wkt = acc.getInterpretation<std::string>()->makePersistent();
        }
        else
        {
            throw rete::NodeBuilderException("Given wkt-parameter is not a string");
        }
    }
    else /* isVariable */
    {
        auto acc = args[1].getAccessor();
        if (acc)
        {
            auto strInterp = acc->getInterpretation<std::string>();
            if (strInterp)
                wkt = strInterp->makePersistent();
            else
                throw rete::NodeBuilderException(
                        "Variable " + args[1].getVariableName() +
                        " is not a string");
        }
        else
        {
            throw rete::NodeBuilderException(
                    "Variable " + args[1].getVariableName() +
                    " is unbound. Must be a string.");
        }
    }
    // create the node
    auto node = std::make_shared<GeoFromWKTNode>(std::move(wkt));

    // bind the result variable to an accessor matching the return of GeoFromWKTNode
    auto resultAccessor = std::make_shared<rete::TupleWME<GeosGeometryInterface::Ptr>::Accessor<0>>();

    args[0].bind(resultAccessor);

    return node;
}


}
