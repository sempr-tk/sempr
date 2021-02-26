#include "nodes/GeoDifferenceNode.hpp"

#include <rete-core/TupleWME.hpp>

namespace sempr {

GeoDifferenceNode::GeoDifferenceNode(
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1,
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo2)
    :
        rete::Builtin("geo:difference"),
        geo1_(std::move(geo1)),
        geo2_(std::move(geo2))
{
}


rete::WME::Ptr GeoDifferenceNode::process(rete::Token::Ptr token)
{
    GeosGeometryInterface::Ptr g1, g2;
    geo1_.interpretation->getValue(token, g1);
    geo2_.interpretation->getValue(token, g2);

    if (!g1->geometry() || !g2->geometry()) return nullptr;

    auto difference = g1->geometry()->difference(g2->geometry());
    auto resultGeom = std::make_shared<GeosGeometry>(std::move(difference));
    auto wme = std::make_shared<rete::TupleWME<GeosGeometryInterface::Ptr>>(resultGeom);
    return wme;
}

bool GeoDifferenceNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const GeoDifferenceNode*>(&other);
    if (!o) return false;

    return *(o->geo1_.accessor) == *(this->geo1_.accessor) &&
           *(o->geo2_.accessor) == *(this->geo2_.accessor);
}

}
