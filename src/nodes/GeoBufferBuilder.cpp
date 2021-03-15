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
    if (args.size() != 3) throw rete::NodeBuilderException("Invalid number of arguments (!=3)");
    if (args[0].isConst() || args[0].getAccessor()) throw rete::NodeBuilderException("First argument must be unbound, as it is reserved for the result.");

    // check if geo-param is valid
    if (args[1].isConst()) throw rete::NodeBuilderException("Second argument cannot be constant");

    if (!args[1].getAccessor() ||
        !args[1].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>())
    {
        throw rete::NodeBuilderException("Second argument must be bound to a Geometry");
    }

    // if offset param is const, create accessor for it
    std::unique_ptr<rete::AccessorBase> offsetAcc;
    if (args[2].isConst())
    {
        rete::ConstantAccessor<double> acc(args[2].getAST().toFloat());
        acc.index() = 0;
        offsetAcc.reset(acc.clone());
    }
    // if offset param is variable, check if bound and clone
    else
    {
        if (!args[2].getAccessor())
        {
            throw rete::NodeBuilderException("Third argument must not be unbound.");
        }
        else
        {
            std::unique_ptr<rete::AccessorBase> acc(args[2].getAccessor()->clone());
            offsetAcc = std::move(acc);
            // todo before using it: check if NumberToNumberConversion is valid
        }
    }

    // prepare args for node creation (clone accessors etc)
    auto geoAccClone = args[1].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>()->makePersistent();
    rete::builtin::NumberToNumberConversion<double> offset(std::move(offsetAcc));

    if (!offset)
        throw rete::NodeBuilderException(
                "Offset parameter " + args[2].getVariableName() +
                " cannot be converted to double.");

    // create the node
    auto node = std::make_shared<GeoBufferNode>(std::move(geoAccClone), std::move(offset));

    // bind the result variable to an accessor matching the return of GeoBufferNode
    auto resultAccessor = std::make_shared<rete::TupleWME<GeosGeometryInterface::Ptr>::Accessor<0>>();
    args[0].bind(resultAccessor);

    return node;
}


}
