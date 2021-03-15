#include "nodes/GeoToWKTBuilder.hpp"
#include "nodes/GeoToWKTNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/builtins/NumberToNumberConversion.hpp>

namespace sempr {


GeoToWKTBuilder::GeoToWKTBuilder()
    : rete::NodeBuilder("geo:toWKT", rete::NodeBuilder::BuilderType::BUILTIN)
{
}


rete::Builtin::Ptr GeoToWKTBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 2 args: result, geo
    if (args.size() != 2) throw rete::NodeBuilderException("Invalid number of arguments (!=2)");
    if (args[0].isConst() || args[0].getAccessor()) throw rete::NodeBuilderException("First argument must be unbound, as it is reserved for the result.");

    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo;
    if (args[1].isConst())
    {
        throw rete::NodeBuilderException("Second parameter must be a geometry.");
    }
    else /* isVariable */
    {
        auto acc = args[1].getAccessor();
        if (acc)
        {
            auto geoInterp = acc->getInterpretation<GeosGeometryInterface::Ptr>();
            if (geoInterp)
                geo = geoInterp->makePersistent();
            else
                throw rete::NodeBuilderException(
                        "Variable " + args[1].getVariableName() +
                        " is not a GeosGeometryInterface");
        }
        else
        {
            throw rete::NodeBuilderException(
                    "Variable " + args[1].getVariableName() +
                    " is unbound. Must be a GeosGeometryInterface");
        }
    }
    // create the node
    auto node = std::make_shared<GeoToWKTNode>(std::move(geo));

    // bind the result variable to an accessor matching the return of GeoToWKTNode
    auto resultAccessor = std::make_shared<rete::TupleWME<std::string>::Accessor<0>>();

    args[0].bind(resultAccessor);

    return node;
}


}
