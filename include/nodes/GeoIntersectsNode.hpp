#ifndef SEMPR_GEOINTERSECTSNODE_HPP_
#define SEMPR_GEOINTERSECTSNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

#include "../component/GeosGeometry.hpp"

namespace sempr {

/**
    Checks if two geometries intersect
*/
class GeoIntersectsNode : public rete::Builtin {
public:
    GeoIntersectsNode(
            rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1,
            rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo2
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1_, geo2_;
};

}

#endif /* include guard: SEMPR_GEOINTERSECTSNODE_HPP_ */
