#include "nodes/GeoAreaNode.hpp"
#include <rete-core/TupleWME.hpp>

namespace sempr {

GeoAreaNode::GeoAreaNode(
        rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo)
    :
        rete::Builtin("geo:area"),
        geo_(std::move(geo))
{
}


rete::WME::Ptr GeoAreaNode::process(rete::Token::Ptr token)
{
    GeosGeometryInterface::Ptr geo;
    geo_.interpretation->getValue(token, geo);

    if (!geo) return nullptr;

    double area = geo->geometry()->getArea();
    auto wme = std::make_shared<rete::TupleWME<double>>(area);
    return wme;
}

bool GeoAreaNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const GeoAreaNode*>(&other);
    if (!o) return false;

    return *(o->geo_.accessor) == *(this->geo_.accessor);
}

}
