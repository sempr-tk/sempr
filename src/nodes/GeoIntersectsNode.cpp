#include "nodes/GeoIntersectsNode.hpp"

#include <rete-core/TupleWME.hpp>

namespace sempr {

GeoIntersectsNode::GeoIntersectsNode(
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1,
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo2)
    :
        rete::Builtin("geo:intersects"),
        geo1_(std::move(geo1)),
        geo2_(std::move(geo2))
{
}


rete::WME::Ptr GeoIntersectsNode::process(rete::Token::Ptr token)
{
    GeosGeometryInterface::Ptr g1, g2;
    geo1_.interpretation->getValue(token, g1);
    geo2_.interpretation->getValue(token, g2);

    if (!g1->geometry() || !g2->geometry()) return nullptr;

    bool ok = g1->geometry()->intersects(g2->geometry());

    if (ok)
        return std::make_shared<rete::EmptyWME>();
    else
        return nullptr;
}

bool GeoIntersectsNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const GeoIntersectsNode*>(&other);
    if (!o) return false;

    return *(o->geo1_.accessor) == *(this->geo1_.accessor) &&
           *(o->geo2_.accessor) == *(this->geo2_.accessor);
}

}
