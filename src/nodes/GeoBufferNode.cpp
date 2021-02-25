#include "nodes/GeoBufferNode.hpp"

#include <rete-core/TupleWME.hpp>

namespace sempr {

GeoBufferNode::GeoBufferNode(
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo,
    rete::builtin::NumberToNumberConversion<double> offset)
    :
        rete::Builtin("geo:buffer"),
        geo_(std::move(geo)),
        offset_(std::move(offset))
{
}


rete::WME::Ptr GeoBufferNode::process(rete::Token::Ptr token)
{
    GeosGeometryInterface::Ptr geo;
    double offset;

    geo_.interpretation->getValue(token, geo);
    offset_.getValue(token, offset);

    if (!geo->geometry()) return nullptr;

    auto buffer = geo->geometry()->buffer(offset);
    auto resultGeom = std::make_shared<GeosGeometry>(std::move(buffer));
    auto wme = std::make_shared<rete::TupleWME<GeosGeometryInterface::Ptr>>(resultGeom);
    return wme;
}

bool GeoBufferNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const GeoBufferNode*>(&other);
    if (!o) return false;

    return *(o->geo_.accessor) == *(this->geo_.accessor) &&
            offset_.hasEqualAccessor(o->offset_);
}

}
