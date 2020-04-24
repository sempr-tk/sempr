#include "nodes/GeoDistanceNode.hpp"

#include <rete-core/TupleWME.hpp>

namespace sempr {

GeoDistanceNode::GeoDistanceNode(
    std::unique_ptr<GeoDistanceNode::accessor_t> geo1,
    std::unique_ptr<GeoDistanceNode::accessor_t> geo2
    )
    : rete::Builtin("geo:distance"),
      geo1_(std::move(geo1)),
      geo2_(std::move(geo2))
{
}

rete::WME::Ptr GeoDistanceNode::process(rete::Token::Ptr token)
{
    GeosGeometryInterface::Ptr g1, g2;
    geo1_->getValue(token, g1);
    geo2_->getValue(token, g2);

    // no result to pass on if any of the geometries is a nullptr
    if (!g1->geometry() || !g2->geometry()) return nullptr;

    // distance between empty geometries?! no result.
    if (g1->geometry()->isEmpty() || g2->geometry()->isEmpty()) return nullptr;

    // compute distance
    double dist = g1->geometry()->distance(g2->geometry());

    // create a wme with the result
    auto wme = std::make_shared<rete::TupleWME<double>>(dist);
    return wme;
}


bool GeoDistanceNode::operator==(const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const GeoDistanceNode*>(&other);
    if (!o) return false;

    return *(o->geo1_) == *(this->geo1_) &&
           *(o->geo2_) == *(this->geo2_);
}



}
