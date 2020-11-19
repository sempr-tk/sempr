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
    if (args.size() != 2)
    {
        throw rete::NodeBuilderException(
                "Need exactly 2 arguments: An unbound"
                " variable for the result, and a group of geometries.");
    }
    else if (!args[0].isVariable() || args[0].getAccessor())
    {
        throw rete::NodeBuilderException(
               "The first argument must be an unbound variable for the result");
    }
    else if (!args[1].isVariable() || !args[1].getAccessor() ||
             !args[1].getAccessor()->getInterpretation<rete::TokenGroup::Ptr>() ||
             !args[1].getAccessor()
                ->getInterpretation<rete::TokenGroup::Ptr>()
                ->getChildInterpretation<GeosGeometryInterface::Ptr>())
    {
        throw rete::NodeBuilderException(
                "The second argument must point to floats in a token group"
                " as created by a GROUP BY statement");
    }

    // get the input interpretation
    auto interp = args[1].getAccessor()
                    ->getInterpretation<rete::TokenGroup::Ptr>()
                    ->makePersistent();

    auto node = std::make_shared<GeoUnionAllNode>(std::move(interp));
    auto result = std::make_shared<
                    rete::TupleWME<GeosGeometryInterface::Ptr>::Accessor<0>
                  >();
    args[0].bind(result);
    return node;
}

}
