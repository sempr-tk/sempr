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
    rete::util::requireNumberOfArgs(args, 2);
    auto geo1 = rete::util::requireInterpretation<GeosGeometryInterface::Ptr>(args, 0);
    auto geo2 = rete::util::requireInterpretation<GeosGeometryInterface::Ptr>(args, 1);

    // create the node
    auto node = std::make_shared<GeoIntersectsNode>(std::move(geo1), std::move(geo2));
    return node;
}

}
