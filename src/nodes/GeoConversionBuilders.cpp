#include "nodes/GeoConversionNodes.hpp"
#include "nodes/GeoConversionBuilders.hpp"

#include <rete-reasoner/Exceptions.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/TupleWMEAccessor.hpp>
#include "TupleComponentAccessor.hpp"

namespace sempr {

UTMFromWGSBuilder::UTMFromWGSBuilder()
    : rete::NodeBuilder("geo:UTMFromWGS", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr UTMFromWGSBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 3 arguments needed
    if (args.size() != 3) throw rete::NodeBuilderException("Invalid number of arguments (!= 3)");
    // first must be unbound for the result
    if (args[0].isConst() || args[0].getAccessor()) throw rete::NodeBuilderException("First argument must be an unbound variable to store the result in");

    // second must be a geometry
    if (args[1].isConst() || !args[1].getAccessor() ||
        !args[1].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>())
    {
        throw rete::NodeBuilderException("Second argument must be a geometry");
    }
    // clone accessor
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo(
        args[1].getAccessor()->getInterpretation<GeosGeometryInterface::Ptr>()->makePersistent()
    );

    // third argument must be an number
    rete::PersistentInterpretation<int> zone;
    if (args[2].isConst())
    {
        // create accessor
        rete::ConstantAccessor<int> acc(args[2].getAST().toFloat());
        acc.index() = 0;
        zone = acc.getInterpretation<int>()->makePersistent();
    }
    else
    {
        if (!args[2].getAccessor() || !args[2].getAccessor()->getInterpretation<int>())
        {
            throw rete::NodeBuilderException("Third argument must be an integer indicating the target zone");
        }

        zone = args[2].getAccessor()->getInterpretation<int>()->makePersistent();
    }

    // create the node
    auto node = std::make_shared<UTMFromWGSNode>(std::move(geo), std::move(zone));

    // create an accessor for the result
    auto resultAccessor = std::make_shared<rete::TupleWMEAccessor<0, rete::TupleWME<GeosGeometryInterface::Ptr>>>();
    args[0].bind(resultAccessor);

    return node;
}

}
