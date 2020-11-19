#ifndef SEMPR_GEOINTERSECTIONNODE_HPP_
#define SEMPR_GEOINTERSECTIONNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

#include "../component/GeosGeometry.hpp"

namespace sempr {

/**
    Computes the intersection of two geometries
*/
class GeoIntersectionNode : public rete::Builtin {
public:
    GeoIntersectionNode(
            rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1,
            rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo2
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1_, geo2_;
};

}

#endif /* include guard: SEMPR_GEOINTERSECTIONNODE_HPP_ */
