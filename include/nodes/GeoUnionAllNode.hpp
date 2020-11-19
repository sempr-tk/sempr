#ifndef SEMPR_GEOUNIONALLNODE_HPP_
#define SEMPR_GEOUNIONALLNODE_HPP_

#include <rete-core/Builtin.hpp>
#include <rete-core/Accessors.hpp>
#include <rete-core/TokenGroupAccessor.hpp>

#include "../component/GeosGeometry.hpp"

namespace sempr {

class GeoUnionAllNode : public rete::Builtin {
public:
    using Ptr = std::shared_ptr<GeoUnionAllNode>;

    GeoUnionAllNode(
        rete::PersistentInterpretation<rete::TokenGroup::Ptr> input
    );

    rete::WME::Ptr process(rete::Token::Ptr) override;
    bool operator == (const rete::BetaNode& other) const override;
private:
    rete::PersistentInterpretation<rete::TokenGroup::Ptr> input_; // group
    const rete::Interpretation<GeosGeometryInterface::Ptr>* geo_; // group-internal

};


}

#endif /* include guard: SEMPR_GEOUNIONALLNODE_HPP_ */
