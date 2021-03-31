#include "nodes/GeoUnionAllBuilder.hpp"
#include "nodes/GeoUnionAllNode.hpp"

#include <rete-reasoner/Exceptions.hpp>
#include <rete-core/TupleWMEAccessor.hpp>
#include <rete-core/TupleWME.hpp>

namespace sempr {

GeoUnionAllBuilder::GeoUnionAllBuilder()
    : rete::NodeBuilder("geo:unionAll", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr GeoUnionAllBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    rete::util::requireNumberOfArgs(args, 2);
    rete::util::requireUnboundVariable(args, 0);

    auto interp = rete::util::requireInterpretation<rete::TokenGroup::Ptr>(args, 1);

    if (!interp.interpretation->getChildInterpretation<GeosGeometryInterface::Ptr>())
    {
        throw rete::NodeBuilderException(
                "The second argument must point to floats in a token group"
                " as created by a GROUP BY statement");
    }

    auto node = std::make_shared<GeoUnionAllNode>(std::move(interp));
    auto result = std::make_shared<
                    rete::TupleWME<GeosGeometryInterface::Ptr>::Accessor<0>
                  >();
    args[0].bind(result);
    return node;
}

}
