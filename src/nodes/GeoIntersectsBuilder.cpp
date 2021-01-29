#include "nodes/GeoIntersectsBuilder.hpp"
#include "nodes/GeoIntersectsNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-reasoner/Exceptions.hpp>


namespace sempr {

GeoIntersectsBuilder::GeoIntersectsBuilder()
    : rete::NodeBuilder("geo:intersects", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr GeoIntersectsBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 3 args: result, geo1, geo2
    if (args.size() != 2) throw rete::NodeBuilderException("Invalid number of arguments (!=2)");

    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1, geo2;

    if (args[0].isConst() || args[1].isConst())
        throw rete::NodeBuilderException("Arguments cannot be constants");

    if (!args[0].getAccessor() ||
        !args[0].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>())
    {
        throw rete::NodeBuilderException(
                "First argument must be bound to a Geometry");
    }

    if (!args[1].getAccessor() ||
        !args[1].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>())
    {
        throw rete::NodeBuilderException(
                "Second argument must be bound to a Geometry");
    }

    // clone accessors
    geo1 = args[0].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>()->makePersistent();
    geo2 = args[1].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>()->makePersistent();

    // create the node
    auto node = std::make_shared<GeoIntersectsNode>(std::move(geo1), std::move(geo2));
    return node;
}

}
