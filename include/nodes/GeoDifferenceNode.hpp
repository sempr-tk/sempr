#ifndef SEMPR_GEODIFFERENCENODE_HPP_
#define SEMPR_GEODIFFERENCENODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>

#include "../component/GeosGeometry.hpp"

namespace sempr {

/**
    Computes the result of geo1 - geo2
*/
class GeoDifferenceNode : public rete::Builtin {
public:
    GeoDifferenceNode(
            rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1,
            rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo2
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;

private:
    rete::PersistentInterpretation<GeosGeometryInterface::Ptr> geo1_, geo2_;
};

}

#endif /* include guard: SEMPR_GEODIFFERENCENODE_HPP_ */
