#include "nodes/GeoBufferBuilder.hpp"
#include "nodes/GeoBufferNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/builtins/NumberToNumberConversion.hpp>

namespace sempr {


GeoBufferBuilder::GeoBufferBuilder()
    : rete::NodeBuilder("geo:buffer", rete::NodeBuilder::BuilderType::BUILTIN)
{
}


rete::Builtin::Ptr GeoBufferBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 3 args: result, geo, offset
    rete::util::requireNumberOfArgs(args, 3);
    rete::util::requireUnboundVariable(args, 0);
    auto geoAccClone = rete::util::requireInterpretation<GeosGeometryInterface::Ptr>(args, 1);
    auto offset = rete::util::requireNumberToNumberConversion<double>(args, 2);

    // create the node
    auto node = std::make_shared<GeoBufferNode>(std::move(geoAccClone), std::move(offset));

    // bind the result variable to an accessor matching the return of GeoBufferNode
    auto resultAccessor = std::make_shared<rete::TupleWME<GeosGeometryInterface::Ptr>::Accessor<0>>();
    args[0].bind(resultAccessor);

    return node;
}


}
