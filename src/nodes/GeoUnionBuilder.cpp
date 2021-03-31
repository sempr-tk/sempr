#include "nodes/GeoUnionBuilder.hpp"
#include "nodes/GeoUnionNode.hpp"

#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-reasoner/Exceptions.hpp>


namespace sempr {

GeoUnionBuilder::GeoUnionBuilder()
    : rete::NodeBuilder("geo:union", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr GeoUnionBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 3 args: result, geo1, geo2
    rete::util::requireNumberOfArgs(args, 3);
    rete::util::requireUnboundVariable(args, 0);

    auto geo1 = rete::util::requireInterpretation<GeosGeometryInterface::Ptr>(args, 1);
    auto geo2 = rete::util::requireInterpretation<GeosGeometryInterface::Ptr>(args, 2);

    // clone accessors
    geo1 = args[1].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>()->makePersistent();
    geo2 = args[2].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>()->makePersistent();

    // create the node
    auto node = std::make_shared<GeoUnionNode>(std::move(geo1), std::move(geo2));

    // bind the result variable to an accessor matching the return of GeoDistanceNode
    auto resultAccessor =
        std::make_shared<
                rete::TupleWME<GeosGeometryInterface::Ptr>::Accessor<0>
        >();

    args[0].bind(resultAccessor);
    return node;
}

}
