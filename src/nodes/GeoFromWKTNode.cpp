#include "nodes/GeoFromWKTNode.hpp"

#include <rete-core/TupleWME.hpp>
#include <geos/io/ParseException.h>

namespace sempr {

GeoFromWKTNode::GeoFromWKTNode(
    rete::PersistentInterpretation<std::string> wkt)
    :
        rete::Builtin("geo:fromWKT"),
        wkt_(std::move(wkt)),
        wktReader_(geos::geom::GeometryFactory::getDefaultInstance())
{
}


rete::WME::Ptr GeoFromWKTNode::process(rete::Token::Ptr token)
{
    std::string wkt;
    wkt_.interpretation->getValue(token, wkt);

    try {
        auto geo = wktReader_.read(wkt);
        if (geo)
        {
            auto resultGeom = std::make_shared<GeosGeometry>(std::move(geo));
            auto wme = std::make_shared<rete::TupleWME<GeosGeometryInterface::Ptr>>(resultGeom);
            return wme;
        }
    } catch (geos::io::ParseException e) {
    }

    return nullptr;
}

bool GeoFromWKTNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const GeoFromWKTNode*>(&other);
    if (!o) return false;

    return *(o->wkt_.accessor) == *(this->wkt_.accessor);
}

}
