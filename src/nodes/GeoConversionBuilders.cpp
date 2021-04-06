#include "nodes/GeoConversionNodes.hpp"
#include "nodes/GeoConversionBuilders.hpp"

#include <rete-reasoner/Exceptions.hpp>
#include <rete-core/TupleWME.hpp>
#include <rete-core/TupleWMEAccessor.hpp>
#include "TupleComponentAccessor.hpp"

namespace sempr {

// ----------------------------------------------------------------------------
// UTM from WGS
// ----------------------------------------------------------------------------

UTMFromWGSBuilder::UTMFromWGSBuilder()
    : rete::NodeBuilder("geo:UTMFromWGS", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr UTMFromWGSBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 3 arguments needed
    rete::util::requireNumberOfArgs(args, 3);
    // first must be unbound for the result
    rete::util::requireUnboundVariable(args, 0);
    // second must be a geometry
    auto geo = rete::util::requireInterpretation<GeosGeometryInterface::Ptr>(args, 1);

    // third argument must be an integer
    auto zone = rete::util::requireInterpretation<int>(args, 2);

    // create the node
    auto node = std::make_shared<UTMFromWGSNode>(std::move(geo), std::move(zone));

    // create an accessor for the result
    auto resultAccessor = std::make_shared<rete::TupleWMEAccessor<0, rete::TupleWME<GeosGeometryInterface::Ptr>>>();
    args[0].bind(resultAccessor);

    return node;
}


// ----------------------------------------------------------------------------
// WGS from UTM
// ----------------------------------------------------------------------------

WGSFromUTMBuilder::WGSFromUTMBuilder()
    : rete::NodeBuilder("geo:WGSFromUTM", rete::NodeBuilder::BuilderType::BUILTIN)
{
}

rete::Builtin::Ptr WGSFromUTMBuilder::buildBuiltin(rete::ArgumentList& args) const
{
    // exactly 3 arguments needed
    rete::util::requireNumberOfArgs(args, 3);

    // first must be unbound for the result
    rete::util::requireUnboundVariable(args, 0);

    // second must be the geometry
    auto geo = rete::util::requireInterpretation<GeosGeometryInterface::Ptr>(args, 1);

    // third argument must be an number
    auto zone = rete::util::requireInterpretation<int>(args, 2);

    // create the node
    auto node = std::make_shared<WGSFromUTMNode>(std::move(geo), std::move(zone));

    // create an accessor for the result
    auto resultAccessor = std::make_shared<rete::TupleWMEAccessor<0, rete::TupleWME<GeosGeometryInterface::Ptr>>>();
    args[0].bind(resultAccessor);

    return node;
}


}
