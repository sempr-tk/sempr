#include "nodes/GeoConversionNodes.hpp"
#include "util/GenericCoordinateFilter.hpp"
#include <rete-core/TupleWME.hpp>

#include <GeographicLib/UTMUPS.hpp>

namespace sempr {


UTMFromWGSNode::UTMFromWGSNode(
        rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo,
        rete::PersistentInterpretation<int> zone)
    :
        rete::Builtin("geo:UTMFromWGS"),
        geo_(std::move(geo)),
        zone_(std::move(zone))
{
}

rete::WME::Ptr UTMFromWGSNode::process(rete::Token::Ptr token)
{
    GeosGeometryInterface::Ptr geometry;
    int targetZone;
    geo_.interpretation->getValue(token, geometry);
    zone_.interpretation->getValue(token, targetZone);

    if (!geometry->geometry()) return nullptr;

    // create a coordinate filter which will apply the transformation
    GenericCoordinateFilter filter(
        [targetZone](geos::geom::Coordinate* coordinate) -> void
        {
            double lon = coordinate->x;
            double lat = coordinate->y;
            bool north;
            int zone;
            GeographicLib::UTMUPS::Forward(
                lat, lon, // wgs84 coordinate input
                zone, north, // computed zone and hemisphere output
                coordinate->x, coordinate->y, // computed coordinates output
                targetZone // explicitely use the chosen zone
            );

            // There is a discontinuity when switching between northern and
            // southern hemisphere. To avoid that, always transfer into the
            // northern hemisphere.
            GeographicLib::UTMUPS::Transfer(
                zone, north, coordinate->x, coordinate->y, // from
                zone, true, coordinate->x, coordinate->y, // to
                zone // the actual zone. I don't really understand this, but
                // this is how the example did it. See:
                // https://geographiclib.sourceforge.io/html/classGeographicLib_1_1UTMUPS.html#a921d6c23e728e0b17651902d43fb56e8
            );
        }
    );

    // create a clone of the geometry
    auto copy = geometry->geometry()->clone();
    // apply the transformation to it
    copy->apply_rw(&filter);
    // put it into a geometry component
    auto component = std::make_shared<GeosGeometry>(std::move(copy));
    // put that into a WME
    auto wme = std::make_shared<rete::TupleWME<GeosGeometryInterface::Ptr>>(component);

    return wme;
}


bool UTMFromWGSNode::operator== (const rete::BetaNode& other) const
{
    auto o = dynamic_cast<const UTMFromWGSNode*>(&other);
    if (!o) return false;

    return *(o->geo_.accessor) == *(this->geo_.accessor) &&
           *(o->zone_.accessor) == *(this->zone_.accessor);
}




}
