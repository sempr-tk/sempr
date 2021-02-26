#ifndef SEMPR_GEOFROMWKTNODE_HPP_
#define SEMPR_GEOFROMWKTNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

#include "../component/GeosGeometry.hpp"

namespace sempr {

/**
    Loads a geometry from a WKT string
*/
class GeoFromWKTNode : public rete::Builtin {
public:
    GeoFromWKTNode(
        rete::PersistentInterpretation<std::string> wkt
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<std::string> wkt_;
    geos::io::WKTReader wktReader_;
};

}

#endif /* include guard: SEMPR_GEOFROMWKTNODE_HPP_ */
