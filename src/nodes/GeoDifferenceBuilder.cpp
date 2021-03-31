#include "nodes/GeoDifferenceBuilder.hpp"
#include "nodes/GeoDifferenceNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-reasoner/Exceptions.hpp>


namespace sempr {

GeoDifferenceBuilder::GeoDifferenceBuilder()
    : rete::NodeBuilder("geo:difference", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr GeoDifferenceBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 3 args: result, geo1, geo2
    rete::util::requireNumberOfArgs(args, 3);
    rete::util::requireUnboundVariable(args, 0);

    auto geo1 = rete::util::requireInterpretation<GeosGeometryInterface::Ptr>(args, 1);
    auto geo2 = rete::util::requireInterpretation<GeosGeometryInterface::Ptr>(args, 2);

    // create the node
    auto node = std::make_shared<GeoDifferenceNode>(std::move(geo1), std::move(geo2));

    // bind the result variable to an accessor matching the return of GeoDistanceNode
    auto resultAccessor =
        std::make_shared<
                rete::TupleWME<GeosGeometryInterface::Ptr>::Accessor<0>
        >();

    args[0].bind(resultAccessor);
    return node;
}

}
