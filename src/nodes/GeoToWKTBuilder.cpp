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
    rete::util::requireNumberOfArgs(args, 2);
    rete::util::requireUnboundVariable(args, 0);
    auto geo = rete::util::requireInterpretation<GeosGeometryInterface::Ptr>(args, 1);

    // create the node
    auto node = std::make_shared<GeoToWKTNode>(std::move(geo));

    // bind the result variable to an accessor matching the return of GeoToWKTNode
    auto resultAccessor = std::make_shared<rete::TupleWME<std::string>::Accessor<0>>();

    args[0].bind(resultAccessor);

    return node;
}


}
