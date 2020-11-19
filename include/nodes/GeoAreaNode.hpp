#ifndef SEMPR_GEOAREANODE_HPP_
#define SEMPR_GEOAREANODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

#include "../component/GeosGeometry.hpp"

namespace sempr {


/**
    Computes the area of a given geometry
*/
class GeoAreaNode : public rete::Builtin {
public:
    GeoAreaNode(
        rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo_;
};


}

#endif /* include guard: SEMPR_GEOAREANODE_HPP_ */
