#include "nodes/GeoAreaBuilder.hpp"
#include "nodes/GeoAreaNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-reasoner/Exceptions.hpp>

namespace sempr {

GeoAreaBuilder::GeoAreaBuilder()
    : rete::NodeBuilder("geo:area", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr GeoAreaBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // need exactly 2 args:
    // [0] unbound var for result
    // [1] var bound to geometry
    if (args.size() != 2)
        throw rete::NodeBuilderException(
                "Invalid number of arguments! "
                "Need exactly one unbound variable for the result, and a "
                "variable bound to the geometry to measure. (In that order)");

    if (args[0].isConst() || args[0].getAccessor())
        throw rete::NodeBuilderException("First argument must be an unbound variable.");


    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo;
    if (args[1].isConst() ||
        !args[1].getAccessor() ||
        !args[1].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>())
    {
        throw rete::NodeBuilderException("Second argument must be bound to a Geometry");
    }

    geo = args[1].getAccessor()
            ->getInterpretation<GeosGeometryInterface::Ptr>()
            ->makePersistent();

    auto node = std::make_shared<GeoAreaNode>(std::move(geo));
    auto resultAccessor = std::make_shared<rete::TupleWMEAccessor<0, rete::TupleWME<double>>>();
    args[0].bind(resultAccessor);

    return node;
}


}
