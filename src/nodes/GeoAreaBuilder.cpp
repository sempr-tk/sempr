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
    rete::util::requireNumberOfArgs(args, 2);
    // [0] unbound var for result
    rete::util::requireUnboundVariable(args, 0);
    // [1] var bound to geometry
    auto geo = rete::util::requireInterpretation<GeosGeometryInterface::Ptr>(args, 1);

    auto node = std::make_shared<GeoAreaNode>(std::move(geo));
    auto resultAccessor = std::make_shared<rete::TupleWMEAccessor<0, rete::TupleWME<double>>>();
    args[0].bind(resultAccessor);

    return node;
}


}
