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
    rete::util::requireNumberOfArgs(args, 2);
    rete::util::requireUnboundVariable(args, 0);

    auto wkt = rete::util::requireInterpretation<std::string>(args, 1);

    // create the node
    auto node = std::make_shared<GeoFromWKTNode>(std::move(wkt));

    // bind the result variable to an accessor matching the return of GeoFromWKTNode
    auto resultAccessor = std::make_shared<rete::TupleWME<GeosGeometryInterface::Ptr>::Accessor<0>>();

    args[0].bind(resultAccessor);

    return node;
}


}
