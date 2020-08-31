#include "nodes/GeoDistanceBuilder.hpp"
#include "nodes/GeoDistanceNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>

namespace sempr {


GeoDistanceBuilder::GeoDistanceBuilder()
    : rete::NodeBuilder("geo:distance", rete::NodeBuilder::BuilderType::BUILTIN)
{
}


rete::Builtin::Ptr GeoDistanceBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 3 args: result, geo1, geo2
    if (args.size() != 3) throw rete::NodeBuilderException("Invalid number of arguments (!=3)");
    if (args[0].isConst() || args[0].getAccessor()) throw rete::NodeBuilderException("First argument must be unbound, as it is reserved for the result.");

    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1, geo2;

    if (args[1].isConst() || args[2].isConst()) throw rete::NodeBuilderException("Arguments cannot be constants");

    if (!args[1].getAccessor() ||
        !args[1].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>())
    {
        throw rete::NodeBuilderException("Second argument must be bound to a Geometry");
    }

    if (!args[2].getAccessor() ||
        !args[2].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>())
    {
        throw rete::NodeBuilderException("Third argument must be bound to a Geometry");
    }

    // clone accessors
    geo1 = args[1].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>()->makePersistent();
    geo2 = args[2].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>()->makePersistent();

    // create the node
    auto node = std::make_shared<GeoDistanceNode>(std::move(geo1), std::move(geo2));

    // bind the result variable to an accessor matching the return of GeoDistanceNode
    auto resultAccessor = std::make_shared<rete::TupleWMEAccessor<0, rete::TupleWME<double>>>();
    args[0].bind(resultAccessor);

    return node;
}


}
