#include "nodes/GeoUnionAllNode.hpp"
#include <rete-core/TupleWME.hpp>
#include <geos/geom/Point.h>

namespace sempr {

GeoUnionAllNode::GeoUnionAllNode(
        rete::PersistentInterpretation<rete::TokenGroup::Ptr> input)
    :
        rete::Builtin("geo::unionAll"),
        input_(std::move(input))
{
    geo_ = input_.interpretation->getChildInterpretation<GeosGeometryInterface::Ptr>();
    if (!geo_)
        throw std::invalid_argument(
                "Accessor does not point to GeosGeometryInterface "
                "in TokenGroup");
}


bool GeoUnionAllNode::operator == (const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const GeoUnionAllNode*>(&other);
    if (!o)
        return false;
    else
        return *o->input_.accessor == *this->input_.accessor;
}


rete::WME::Ptr GeoUnionAllNode::process(rete::Token::Ptr token)
{
    rete::TokenGroup::Ptr group;
    this->input_.interpretation->getValue(token, group);

    // empty geometry as a basis
    auto geometry =
        std::make_shared<GeosGeometry>(
            std::move(
                geos::geom::GeometryFactory::getDefaultInstance()->createPoint()
            )
        );

    // union with all geometries in the group
    for (auto entry : group->token_)
    {
        GeosGeometryInterface::Ptr val;
        geo_->getValue(entry, val);

        geometry->setGeometry(
            std::move(
                geometry->geometry()->Union(val->geometry())
            )
        );
    }

    return std::make_shared<rete::TupleWME<GeosGeometryInterface::Ptr>>(geometry);
}

}
