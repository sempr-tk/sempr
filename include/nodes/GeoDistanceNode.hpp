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
    typedef rete::SpecificTypeAccessor<std::shared_ptr<GeosGeometryInterface>> accessor_t;
    GeoDistanceNode(
            std::unique_ptr<accessor_t> geo1,
            std::unique_ptr<accessor_t> geo2
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;

    bool operator == (const rete::BetaNode& other) const override;

private:
    std::unique_ptr<accessor_t> geo1_, geo2_;

};


}


#endif /* include guard: SEMPR_GEODISTANCENODE_HPP_ */
