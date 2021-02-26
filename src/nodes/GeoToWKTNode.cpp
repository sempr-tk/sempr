#include "nodes/GeoToWKTNode.hpp"

#include <rete-core/TupleWME.hpp>
#include <geos/io/ParseException.h>

namespace sempr {

GeoToWKTNode::GeoToWKTNode(
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo)
    :
        rete::Builtin("geo:toWKT"),
        geo_(std::move(geo))
{
}


rete::WME::Ptr GeoToWKTNode::process(rete::Token::Ptr token)
{
    GeosGeometryInterface::Ptr geo;
    geo_.interpretation->getValue(token, geo);

    if (!geo || !geo->geometry())
        return nullptr;

    std::string wkt = wktWriter_.write(geo->geometry());

    auto wme = std::make_shared<rete::TupleWME<std::string>>(wkt);
    return wme;
}

bool GeoToWKTNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const GeoToWKTNode*>(&other);
    if (!o) return false;

    return *(o->geo_.accessor) == *(this->geo_.accessor);
}

}
