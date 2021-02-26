#ifndef SEMPR_GEOTOWKTNODE_HPP_
#define SEMPR_GEOTOWKTNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

#include "../component/GeosGeometry.hpp"

namespace sempr {

/**
    Creates a WKT representation of the given geometry
*/
class GeoToWKTNode : public rete::Builtin {
public:
    GeoToWKTNode(
        rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo_;
    geos::io::WKTWriter wktWriter_;
};

}

#endif /* include guard: SEMPR_GEOTOWKTNODE_HPP_ */
