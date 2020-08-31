#ifndef SEMPR_GEODISTANCENODE_HPP_
#define SEMPR_GEODISTANCENODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

#include "../component/GeosGeometry.hpp"

namespace sempr {

/**
    A node that computes the distance between two geometries
*/
class GeoDistanceNode : public rete::Builtin {
public:
    GeoDistanceNode(
            rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1,
            rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo2
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;

    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1_, geo2_;

};


}


#endif /* include guard: SEMPR_GEODISTANCENODE_HPP_ */
